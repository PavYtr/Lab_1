#!/usr/bin/env python3
"""Run the real Eco component and libc reference; retain raw data and provenance."""
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
from datetime import datetime, timezone
from zoneinfo import ZoneInfo

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent


def output(args):
    return subprocess.check_output(args, cwd=ROOT, text=True).strip()


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    results = HERE / "results"
    results.mkdir(exist_ok=True)
    compiler = (Path(os.environ["ECO_TOOLCHAIN"]) /
                "eco-x86_64-linux-gnu-gcc-13.2.0/bin/x86_64-linux-gnu-gcc"
                if os.environ.get("ECO_TOOLCHAIN") else Path("gcc"))
    cpu_info = Path("/proc/cpuinfo").read_text()
    cpu_model = next(line.split(":", 1)[1].strip() for line in cpu_info.splitlines()
                     if line.startswith("model name"))
    available = sorted(os.sched_getaffinity(0))
    cpu = available[0]
    now = datetime.now(timezone.utc)
    metadata = {
        "started_utc": now.isoformat(),
        "started_moscow": now.astimezone(ZoneInfo("Europe/Moscow")).isoformat(),
        "platform": platform.platform(),
        "cpu_model": cpu_model,
        "pinned_cpu": cpu,
        "allowed_cpus": available,
        "libc": output(["getconf", "GNU_LIBC_VERSION"]),
        "compiler": output([str(compiler), "--version"]).splitlines()[0],
        "benchmark_flags": "-std=c11 -O2 -DNDEBUG -Wall -Wextra -fno-builtin",
        "component_flags": "-std=c89 -O2 -DNDEBUG -ffreestanding -fno-builtin",
        "timer": "CLOCK_MONOTONIC_RAW",
        "samples_per_case": 9,
        "warmup_pairs_per_case": 1,
        "input_seed_base": 20261010,
        "git_commit": output(["git", "rev-parse", "HEAD"]),
        "sha256": {},
    }
    for relative in ("SourceFiles/CEcoHeapSort.c", "SourceFiles/CEcoHeapSortFactory.c",
                     "SharedFiles/IEcoSortAndSearch.h", "ComparisonFiles/benchmark.c",
                     "ComparisonFiles/Makefile", "ComparisonFiles/build/benchmark",
                     "BuildFiles/Linux/x86_64/StaticRelease/libA3A5BD4760D34C1691719DF4D8D41318.a"):
        metadata["sha256"][relative] = sha256(ROOT / relative)
    original_affinity = os.sched_getaffinity(0)
    partial = results / "raw.csv.partial"
    try:
        # The child inherits one CPU. Restore the runner's affinity afterward.
        os.sched_setaffinity(0, {cpu})
        with partial.open("w") as raw, (results / "run.log").open("w") as log:
            process = subprocess.Popen([str(HERE / "build/benchmark")], cwd=HERE,
                                       stdout=raw, stderr=subprocess.PIPE, text=True)
            for line in process.stderr:
                print(line.rstrip(), flush=True)
                log.write(line)
            status = process.wait()
    finally:
        os.sched_setaffinity(0, original_affinity)
    metadata["exit_code"] = status
    metadata["finished_utc"] = datetime.now(timezone.utc).isoformat()
    if status:
        (results / "environment.failed.json").write_text(json.dumps(metadata, indent=2, ensure_ascii=False) + "\n")
        raise SystemExit(f"Benchmark failed ({status}); incomplete data: {partial}")
    metadata["raw_csv_sha256"] = sha256(partial)
    partial.replace(results / "raw.csv")
    (results / "environment.json").write_text(json.dumps(metadata, indent=2, ensure_ascii=False) + "\n")
    print("Saved results/raw.csv and results/environment.json", flush=True)


if __name__ == "__main__":
    main()
