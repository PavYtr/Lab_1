call ndk-build clean
call ndk-build TARGET_LINK=0 NDK_DEBUG=0 CONFIGURATION=StaticRelease
call ndk-build TARGET_LINK=0 NDK_DEBUG=1 CONFIGURATION=StaticDebug
rmdir /s /q libs
rmdir /s /q obj
call ndk-build clean
call ndk-build TARGET_LINK=1 NDK_DEBUG=0 CONFIGURATION=DynamicRelease
call ndk-build TARGET_LINK=1 NDK_DEBUG=1 CONFIGURATION=DynamicDebug
rmdir /s /q libs
rmdir /s /q obj
rem xcopy /y ..\..\..\..\BuildFiles\Android\arm64-v8a\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\arm64-v8a\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\arm64-v8a\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\arm64-v8a\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\armeabi\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\armeabi\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi-v7a\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\armeabi-v7a\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi-v7a\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\armeabi-v7a\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\mips\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\mips\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips64\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\mips64\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips64\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\mips64\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\x86\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\x86\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86_64\StaticRelease\libA3A5BD4760D34C1691719DF4D8D41318.a %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\x86_64\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86_64\DynamicRelease\libA3A5BD4760D34C1691719DF4D8D41318.so %ECO_FRAMEWORK%\EcoHeapSort\BuildFiles\Android\x86_64\DynamicRelease\
pause
