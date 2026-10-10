#!/usr/bin/env python3
"""Aggregate the recorded measurements, export plots and an editable Word report."""
import csv
import hashlib
from collections import defaultdict
import json
import math
import os
from pathlib import Path
import statistics
import struct
import zipfile
from xml.sax.saxutils import escape

HERE = Path(__file__).resolve().parent
RESULTS = HERE / "results"
FIGURES = HERE / "figures"
os.environ.setdefault("MPLCONFIGDIR", str(HERE / ".cache/matplotlib"))
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import TwoSlopeNorm
from matplotlib.ticker import FuncFormatter

PATTERNS = {
    "random": "Случайные значения",
    "sorted": "Уже отсортированы",
    "reversed": "Обратный порядок",
    "duplicates": "Много повторов (17 ключей)",
    "equal": "Все ключи одинаковые",
}
TYPES = {"int": "int", "long": "long", "float": "float", "double": "double",
         "long_double": "long double", "record": "структура"}
COLORS = {"heap": "#225ea8", "qsort": "#e08214"}
SCENARIO_COLORS = ["#225ea8", "#228b55", "#c45826", "#7a5195", "#525252"]


def quantiles(values):
    q1, _, q3 = statistics.quantiles(values, n=4, method="inclusive")
    return q1, statistics.median(values), q3


def aggregate():
    metadata = json.loads((RESULTS / "environment.json").read_text())
    if metadata["exit_code"] != 0:
        raise ValueError("Benchmark did not complete successfully")
    if (metadata.get("raw_csv_sha256") and metadata["raw_csv_sha256"] !=
            hashlib.sha256((RESULTS / "raw.csv").read_bytes()).hexdigest()):
        raise ValueError("Raw measurements do not match the recorded SHA-256")
    grouped = defaultdict(list)
    with (RESULTS / "raw.csv").open() as stream:
        for row in csv.DictReader(stream):
            for name in ("count", "element_bytes", "batch", "sample", "heap_batch_ns",
                         "qsort_batch_ns", "first_algorithm", "seed"):
                row[name] = int(row[name])
            if min(row["batch"], row["heap_batch_ns"], row["qsort_batch_ns"]) <= 0:
                raise ValueError("Nonpositive measurement")
            grouped[row["type"], row["pattern"], row["count"]].append(row)
    expected = {(kind, pattern, count) for kind in TYPES for pattern in PATTERNS
                for count in (256, 1024, 4096, 16384, 65536, 262144, 1048576)
                if kind == "int" or count <= 262144}
    if set(grouped) != expected:
        raise ValueError("Missing or unexpected benchmark cases")
    summaries = []
    for key, rows in grouped.items():
        if len(rows) != 9 or {row["sample"] for row in rows} != set(range(9)):
            raise ValueError(f"Incomplete paired measurements: {key}")
        summary = dict(zip(("type", "pattern", "count"), key))
        summary["element_bytes"] = rows[0]["element_bytes"]
        summary["samples"] = len(rows)
        for algorithm in ("heap", "qsort"):
            q1, median, q3 = quantiles([row[algorithm + "_batch_ns"] / row["batch"] for row in rows])
            summary[algorithm + "_q1_ns"] = q1
            summary[algorithm + "_median_ns"] = median
            summary[algorithm + "_q3_ns"] = q3
        q1, median, q3 = quantiles([row["heap_batch_ns"] / row["qsort_batch_ns"] for row in rows])
        summary.update(ratio_q1=q1, ratio_median=median, ratio_q3=q3)
        summaries.append(summary)
    with (RESULTS / "summary.csv").open("w") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(summaries[0]))
        writer.writeheader()
        writer.writerows(summaries)
    return summaries


def save(fig, name):
    for extension in ("png", "svg", "pdf"):
        fig.savefig(FIGURES / f"{name}.{extension}", dpi=190, bbox_inches="tight",
                    facecolor="white")
    plt.close(fig)


def series(rows, kind, pattern):
    return sorted((row for row in rows if row["type"] == kind and row["pattern"] == pattern),
                  key=lambda row: row["count"])


def plots(rows):
    FIGURES.mkdir(exist_ok=True)
    plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11,
                         "axes.spines.top": False, "axes.spines.right": False,
                         "axes.titleweight": "bold", "axes.grid": True,
                         "grid.alpha": 0.22, "axes.axisbelow": True,
                         "svg.fonttype": "none"})
    fig, axes = plt.subplots(2, 3, figsize=(15, 9), sharex=True, sharey=True)
    for ax, (pattern, label) in zip(axes.flat, PATTERNS.items()):
        data = series(rows, "int", pattern)
        x = [row["count"] for row in data]
        for algorithm, legend in (("heap", "Eco Heapsort"), ("qsort", "C qsort")):
            ax.plot(x, [row[algorithm + "_median_ns"] / 1e6 for row in data],
                    color=COLORS[algorithm], marker="o", linewidth=2, label=legend)
            ax.fill_between(x, [row[algorithm + "_q1_ns"] / 1e6 for row in data],
                            [row[algorithm + "_q3_ns"] / 1e6 for row in data],
                            color=COLORS[algorithm], alpha=0.17)
        ax.set_xscale("log", base=2)
        ax.set_yscale("log")
        ax.set_title(label, fontsize=12)
        ax.set_xlabel("Количество элементов n")
        ax.set_ylabel("Время одной сортировки, мс")
        ax.set_xticks([256, 4096, 65536, 1048576], ["256", "4 096", "65 536", "1 048 576"])
        ax.tick_params(axis="x", labelsize=9)
    axes.flat[-1].axis("off")
    handles, labels = axes.flat[0].get_legend_handles_labels()
    axes.flat[-1].legend(handles, labels, loc="upper left", frameon=False, fontsize=14)
    axes.flat[-1].text(0.04, 0.65,
        "Как читать\n\nНиже линия → меньше времени.\nЛиния: медиана 9 замеров.\nПолоса: 25–75-й процентили.\n\nОбе оси логарифмические.\nШкалы одинаковы во всех панелях.\nТип: int, 4 байта.",
        transform=axes.flat[-1].transAxes, va="top", linespacing=1.5)
    fig.suptitle("Скорость сортировки int: Heapsort и стандартная qsort", fontsize=18)
    fig.tight_layout(rect=(0, 0, 1, 0.95))
    save(fig, "01_int_time")

    fig, ax = plt.subplots(figsize=(12, 7))
    for (pattern, label), color in zip(PATTERNS.items(), SCENARIO_COLORS):
        data = series(rows, "int", pattern)
        x = [row["count"] for row in data]
        ax.plot(x, [row["ratio_median"] for row in data], marker="o", linewidth=2,
                color=color, label=label)
        ax.fill_between(x, [row["ratio_q1"] for row in data], [row["ratio_q3"] for row in data],
                        color=color, alpha=0.1)
    ax.axhline(1, linestyle="--", color="black", linewidth=1.5)
    ax.set_xscale("log", base=2)
    ticks = [row["count"] for row in series(rows, "int", "random")]
    ax.set_xticks(ticks, [f"{value:,}".replace(",", " ") for value in ticks], rotation=15)
    ax.set_yscale("log", base=2)
    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:g}×"))
    ax.set_xlabel("Количество элементов n")
    ax.set_ylabel("t(Heapsort) / t(qsort), раз")
    ax.set_title("Отношение времени сортировки int", fontsize=17, pad=16)
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1), frameon=False)
    fig.text(0.08, 0.02,
             "Выше 1: быстрее qsort. Например, 2× означает вдвое меньшее время qsort.\n"
             "Ниже 1: быстрее Heapsort. Линия — медиана парных отношений; полоса — 25–75-й процентили.",
             fontsize=11)
    fig.tight_layout(rect=(0, 0.1, 1, 1))
    save(fig, "02_int_ratio")

    selected = [next(row for row in rows if row["type"] == kind and
                     row["pattern"] == "random" and row["count"] == 262144) for kind in TYPES]
    fig, ax = plt.subplots(figsize=(13, 7))
    for algorithm, offset, label in (("heap", -0.2, "Eco Heapsort"), ("qsort", 0.2, "C qsort")):
        medians = [row[algorithm + "_median_ns"] / 1e6 for row in selected]
        errors = [[(row[algorithm + "_median_ns"] - row[algorithm + "_q1_ns"]) / 1e6 for row in selected],
                  [(row[algorithm + "_q3_ns"] - row[algorithm + "_median_ns"]) / 1e6 for row in selected]]
        bars = ax.bar([i + offset for i in range(len(selected))], medians, width=0.36,
                      yerr=errors, capsize=4, color=COLORS[algorithm], label=label)
        ax.bar_label(bars, labels=[f"{value:.1f}" for value in medians], padding=5, fontsize=10)
    ax.set_xticks(range(len(selected)), [f"{TYPES[row['type']]}\n{row['element_bytes']} байт" for row in selected])
    ax.set_ylabel("Время одной сортировки, мс")
    ax.set_ylim(0, max(row["heap_q3_ns"] for row in selected) / 1e6 * 1.2)
    ax.set_title("Влияние типа элемента: 262 144 случайных значения", fontsize=17)
    ax.legend(frameon=False)
    fig.text(0.08, 0.02, "Высота столбца — медиана 9 замеров; усики — 25–75-й процентили.\n"
             "У структуры сравнивается только ключ; сортировка переносит запись целиком.", fontsize=11)
    fig.tight_layout(rect=(0, 0.1, 1, 1))
    save(fig, "03_type_time")

    matrix_rows = [[next(row for row in rows if row["type"] == kind and row["pattern"] == pattern
                         and row["count"] == 262144) for pattern in PATTERNS] for kind in TYPES]
    matrix = [[math.log2(row["ratio_median"]) for row in line] for line in matrix_rows]
    bound = max(abs(value) for line in matrix for value in line)
    fig, ax = plt.subplots(figsize=(13, 7))
    im = ax.imshow(matrix, cmap="coolwarm", norm=TwoSlopeNorm(vcenter=0, vmin=-bound, vmax=bound), aspect="auto")
    ax.set_xticks(range(len(PATTERNS)), ["Случайные", "Отсортированы", "Обратный\nпорядок", "17 ключей", "Один ключ"])
    ax.set_yticks(range(len(TYPES)), [f"{TYPES[line[0]['type']]} ({line[0]['element_bytes']} байт)" for line in matrix_rows])
    ax.grid(False)
    ax.set_title("Heapsort / qsort: отношение времени при n = 262 144", fontsize=17, pad=18)
    for i, line in enumerate(matrix_rows):
        for j, row in enumerate(line):
            ax.text(j, i, f"{row['ratio_median']:.2f}×", ha="center", va="center", fontsize=14,
                    color="white" if abs(matrix[i][j]) > bound * 0.7 else "black")
    colorbar = fig.colorbar(im, ax=ax, shrink=0.86)
    color_ticks = list(range(math.ceil(-bound), math.floor(bound) + 1))
    colorbar.set_ticks(color_ticks, labels=[f"{2 ** value:g}×" for value in color_ticks])
    colorbar.set_label("t(Heapsort) / t(qsort), раз")
    fig.text(0.08, 0.02, "В каждой ячейке — медиана 9 парных отношений времени.\n"
             "Больше 1: быстрее qsort; меньше 1: быстрее Heapsort. 1 означает одинаковое время.", fontsize=11)
    fig.tight_layout(rect=(0, 0.1, 1, 1))
    save(fig, "04_overview_ratio")


def write_docx(blocks, target):
    """Minimal OOXML document: editable text/tables and embedded PNG figures."""
    paragraphs = []
    images = []
    def paragraph(text, style=None):
        properties = f'<w:pPr><w:pStyle w:val="{style}"/></w:pPr>' if style else ""
        return f'<w:p>{properties}<w:r><w:t xml:space="preserve">{escape(text)}</w:t></w:r></w:p>'
    for kind, value in blocks:
        if kind in ("title", "heading", "text"):
            paragraphs.append(paragraph(value, {"title": "Title", "heading": "Heading1"}.get(kind)))
        elif kind == "table":
            cells = []
            for row in value:
                cells.append("<w:tr>" + "".join('<w:tc><w:tcPr><w:tcW w:w="1800" w:type="dxa"/></w:tcPr>' +
                             paragraph(str(cell)) + "</w:tc>" for cell in row) + "</w:tr>")
            borders = "".join(f'<w:{side} w:val="single" w:sz="4" w:color="CCCCCC"/>'
                              for side in ("top", "left", "bottom", "right", "insideH", "insideV"))
            paragraphs.append('<w:tbl><w:tblPr><w:tblW w:w="0" w:type="auto"/><w:tblBorders>' +
                              borders + '</w:tblBorders></w:tblPr>' + "".join(cells) + "</w:tbl>")
        elif kind == "image":
            images.append(value)
            index = len(images)
            width, height = struct.unpack(">II", value.read_bytes()[16:24])
            cx = 5760720
            cy = int(cx * height / width)
            paragraphs.append(f'''<w:p><w:r><w:drawing><wp:inline>
<wp:extent cx="{cx}" cy="{cy}"/><wp:docPr id="{index}" name="Figure {index}"/>
<a:graphic><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:pic><pic:nvPicPr><pic:cNvPr id="{index}" name="{value.name}"/><pic:cNvPicPr/></pic:nvPicPr>
<pic:blipFill><a:blip r:embed="rId{index}"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>
<pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm>
<a:prstGeom prst="rect"><a:avLst/></a:prstGeom></pic:spPr></pic:pic>
</a:graphicData></a:graphic></wp:inline></w:drawing></w:r></w:p>''')
    document = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing"
xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><w:body>''' + "".join(paragraphs) + '''
<w:sectPr><w:pgSz w:w="11906" w:h="16838"/><w:pgMar w:top="1134" w:right="1134" w:bottom="1134" w:left="1134"/></w:sectPr>
</w:body></w:document>'''
    with zipfile.ZipFile(target, "w", zipfile.ZIP_DEFLATED) as archive:
        archive.writestr("[Content_Types].xml", '''<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
<Default Extension="xml" ContentType="application/xml"/><Default Extension="png" ContentType="image/png"/>
<Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
<Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
</Types>''')
        archive.writestr("_rels/.rels", '''<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>''')
        relationships = [f'<Relationship Id="rId{i}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="media/{path.name}"/>'
                         for i, path in enumerate(images, 1)]
        relationships.append('<Relationship Id="styles" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>')
        archive.writestr("word/_rels/document.xml.rels", '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">' +
                         "".join(relationships) + "</Relationships>")
        archive.writestr("word/document.xml", document)
        archive.writestr("word/styles.xml", '''<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
<w:docDefaults><w:rPrDefault><w:rPr><w:rFonts w:ascii="Calibri" w:hAnsi="Calibri"/><w:sz w:val="22"/><w:lang w:val="ru-RU"/></w:rPr></w:rPrDefault></w:docDefaults>
<w:style w:type="paragraph" w:styleId="Title"><w:name w:val="Title"/><w:rPr><w:b/><w:sz w:val="34"/></w:rPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/><w:pPr><w:keepNext/></w:pPr><w:rPr><w:b/><w:sz w:val="27"/></w:rPr></w:style>
</w:styles>''')
        for path in images:
            archive.write(path, "word/media/" + path.name)


def report(rows):
    metadata = json.loads((RESULTS / "environment.json").read_text())
    with (RESULTS / "raw.csv").open() as stream:
        measured_sort_calls = sum(2 * int(row["batch"]) for row in csv.DictReader(stream))
    lookup = {(row["type"], row["pattern"], row["count"]): row for row in rows}
    blocks = []
    def add(kind, content):
        blocks.append((kind, content))
    add("title", "Анализ производительности EcoHeapSort и qsort")
    add("text", "Задание № 60: пирамидальная сортировка. Эксперимент выполнен отдельно от компонента и обязательного UnitTest. Этот материал покрывает анализ времени сортировки для отчётности; он не заменяет полный отчёт по компоненту.")
    add("heading", "1. Стенд и сравниваемые реализации")
    add("table", [["Параметр", "Значение"], ["Дата (Москва)", metadata["started_moscow"]],
                  ["Процессор", metadata["cpu_model"]], ["Система", metadata["platform"]],
                  ["Библиотека C", metadata["libc"]], ["Компилятор", metadata["compiler"]],
                  ["Конфигурация", "Release, -O2; компонент -ffreestanding -fno-builtin"],
                  ["Закреплённый логический CPU", metadata["pinned_cpu"]],
                  ["Таймер", "clock_gettime(CLOCK_MONOTONIC_RAW), наносекунды"]])
    add("text", "Heapsort вызывается через настоящий IEcoSortAndSearch, полученный через QueryComponent, и берётся из Release-архива проекта. Эталон — qsort установленной библиотеки C. Обе реализации получают один и тот же компаратор. Название qsort не гарантирует конкретный внутренний алгоритм: стандарт C задаёт поведение, а реализация зависит от библиотеки и её версии.")
    add("heading", "2. Методика измерений")
    add("text", "Пять сценариев: случайные значения, уже отсортированный массив, обратный порядок, 17 различных ключей, все ключи одинаковые. Типы: int, long, float, double, long double и структура из 32 байт (ключ, уникальный id и 24 байта полезных данных). NaN не используются; дробные значения кратны 0,25 и представимы точно.")
    add("text", f"Размеры: 256, 1 024, 4 096, 16 384, 65 536, 262 144; для int также 1 048 576. Итого 185 сценариев, 9 парных замеров на сценарий — 1 665 пар, или 3 330 измерений времени (часть — серии сортировок). Внутри этих измерений выполнено {measured_sort_calls:,} вызовов сортировки без учёта прогрева. Перед каждой сортировкой восстанавливается исходный массив. Генерация, выделение памяти, копирование, проверки и создание Eco-компонента не входят в измеряемое время.")
    add("text", "Перед замерами выполнена одна прогревочная пара. Порядок запуска Heapsort/qsort чередуется. Для n < 8 192 измеряется серия независимых сортировок свежих копий: время серии делится на число сортировок. Для остальных размеров измеряется одна сортировка. Зафиксирован воспроизводимый seed; процесс закреплён за одним доступным CPU. На графиках приведены медианы и интерквартильные диапазоны (25–75-й процентили), которые показывают разброс, а не доверительный интервал.")
    add("text", "В пределах одного сценария оба алгоритма и все повторы получают одинаковые входные данные. Случайный seed зависит от типа и размера: между типами сравниваются одинаковые распределения, но разные конкретные последовательности. График типов поэтому показывает стоимость на выбранных выборках, а не изолированное влияние одного размера элемента.")
    add("text", "После каждого запуска проверяется порядок. Для числовых типов результат побайтово сопоставляется с qsort. Для структур проверяются порядок ключей, уникальность id и точное совпадение всей записи с исходной по id; порядок равных ключей может различаться. Все проверки завершились успешно, код клиента — 0.")
    add("heading", "3. Численные результаты")
    add("text", "Отношение R = t(Heapsort) / t(qsort). R > 1 означает преимущество qsort; R < 1 — преимущество Heapsort. В таблицах R — медиана отношений внутри пар, поэтому она может слегка отличаться от частного двух отдельно вычисленных медиан времени.")
    int_table = [["int, n = 1 048 576", "Heapsort, мс", "qsort, мс", "R"]]
    for pattern, label in PATTERNS.items():
        row = lookup["int", pattern, 1048576]
        int_table.append([label, f"{row['heap_median_ns'] / 1e6:.3f}",
                          f"{row['qsort_median_ns'] / 1e6:.3f}", f"{row['ratio_median']:.2f}"])
    add("table", int_table)
    type_table = [["Случайные, n = 262 144", "Байт", "Heapsort, мс", "qsort, мс", "R"]]
    for kind, label in TYPES.items():
        row = lookup[kind, "random", 262144]
        type_table.append([label, row["element_bytes"], f"{row['heap_median_ns'] / 1e6:.3f}",
                           f"{row['qsort_median_ns'] / 1e6:.3f}", f"{row['ratio_median']:.2f}"])
    add("table", type_table)
    add("heading", "4. Графики и их интерпретация")
    figures = [
        ("01_int_time", "Рисунок 1. Время сортировки int в пяти сценариях. Обе оси логарифмические; шкалы панелей одинаковы. Чем ниже линия, тем быстрее сортировка. Полосы показывают разброс центральных 50% замеров."),
        ("02_int_ratio", "Рисунок 2. Отношение времени Heapsort/qsort для int. Горизонтальная линия 1 соответствует равному времени. Например, значение 2 означает, что qsort заняла вдвое меньше времени."),
        ("03_type_time", "Рисунок 3. Сравнение типов на случайных данных при одинаковом числе элементов. Столбцы показывают миллисекунды; размер элемента подписан отдельно. Различия включают стоимость компаратора и переноса элемента."),
        ("04_overview_ratio", "Рисунок 4. Сводное отношение времени для всех типов и сценариев при n = 262 144. Числа в ячейках дают прямую интерпретацию; цветовая шкала логарифмическая.")]
    for name, caption in figures:
        add("image", FIGURES / (name + ".png"))
        add("text", caption)
    add("heading", "5. Выводы по фактическим измерениям")
    random = lookup["int", "random", 1048576]
    winner = "qsort" if random["ratio_median"] > 1 else "Heapsort"
    factor = random["ratio_median"] if winner == "qsort" else 1 / random["ratio_median"]
    add("text", f"На случайном массиве из 1 048 576 int быстрее {winner}: отношение времени соответствует преимуществу примерно в {factor:.2f} раза. Медиана Heapsort — {random['heap_median_ns']/1e6:.3f} мс, qsort — {random['qsort_median_ns']/1e6:.3f} мс.")
    for pattern in ("sorted", "reversed", "duplicates", "equal"):
        row = lookup["int", pattern, 1048576]
        add("text", f"{PATTERNS[pattern]}: Heapsort {row['heap_median_ns']/1e6:.3f} мс; qsort {row['qsort_median_ns']/1e6:.3f} мс; медианное парное отношение R = {row['ratio_median']:.2f}.")
    add("text", "На одинаковых ключах текущий Heapsort может выигрывать: в восстановлении кучи сравнение родителя с ребёнком сразу завершает проход при равенстве. Поэтому в таком специальном случае нет длинных спусков, и фактическая работа близка к линейной по числу элементов. Это объяснение поведения собственного кода; внутренние операции qsort в эксперименте отдельно не подсчитывались.")
    wins_heap = sum(row["ratio_median"] < 1 for row in rows)
    add("text", f"По медианному парному отношению Heapsort быстрее в {wins_heap} из {len(rows)} сценариев, qsort — в {sum(row['ratio_median'] > 1 for row in rows)}. Это число описывает именно выбранную сетку данных и не является универсальной оценкой алгоритмов.")
    add("text", "Увеличение времени с ростом n согласуется с обработкой всё большего массива. Сам эксперимент не доказывает асимптотическую сложность. Для текущей реализации Heapsort O(n log n) сравнений и O(1) дополнительной памяти следуют из построения кучи и итеративного восстановления. При переменном размере элемента побайтовые обмены добавляют множитель size к стоимости перемещения. Структуры и long double поэтому могут иметь иную цену сортировки, чем int.")
    add("text", "Причины различий во времени в этом опыте не измерялись отдельно. На них влияют число сравнений, доступ к памяти, размер элемента, компаратор и внутренняя реализация qsort. Графики фиксируют результат сравнения готовых реализаций; они не позволяют приписать весь разрыв одному фактору.")
    add("heading", "6. Ограничения и воспроизводимость")
    add("text", "Измерен один компьютер и одна версия libc. Закрепление CPU не отключает Turbo Boost, изменение частоты, фоновые процессы или активность соседнего SMT-потока. Межквартильный диапазон не охватывает все выбросы. Малые массивы измерены сериями на предварительно скопированных данных; это отличается от холодного запуска. Стоимость выделения памяти внутри qsort, если она есть, входит во время вызова. Пиковая память и аппаратные счётчики не измерялись.")
    add("text", "Все сценарии относятся к возрастанию и согласованным компараторам; NaN, пользовательские дорогие компараторы и другие распределения не исследованы. Стабильность не требуется. Выводы о скорости относятся к этим реализациям в Release на указанном стенде; их нельзя автоматически переносить на другую библиотеку C, архитектуру или Debug.")
    add("text", "Перезапуск: make -C ComparisonFiles run; затем python3 ComparisonFiles/plot.py с установленным Matplotlib из ComparisonFiles/requirements.txt. Исходные измерения: results/raw.csv. Все 185 агрегированных строк: results/summary.csv. Данные стенда, seed и SHA-256 исходников/архива/исполняемого файла: results/environment.json. Графики доступны в PNG, SVG и PDF. libc используется только отдельной программой сравнения; исходники компонента и обязательного UnitTest не изменены.")
    markdown = []
    for kind, value in blocks:
        if kind == "title": markdown.append("# " + value)
        elif kind == "heading": markdown.append("## " + value)
        elif kind == "text": markdown.append(value)
        elif kind == "image": markdown.append(f"![{value.stem}](../figures/{value.name})")
        elif kind == "table":
            lines = ["| " + " | ".join(map(str, row)) + " |" for row in value]
            lines.insert(1, "| " + " | ".join("---" for _ in value[0]) + " |")
            markdown.append("\n".join(lines))
    (RESULTS / "PerformanceReport.md").write_text("\n\n".join(markdown) + "\n")
    write_docx(blocks, RESULTS / "PerformanceReport.docx")


def main():
    rows = aggregate()
    plots(rows)
    report(rows)
    print(f"Validated {len(rows)} scenarios; exported 4 graphs (PNG/SVG/PDF), CSV and Word report.")


if __name__ == "__main__":
    main()
