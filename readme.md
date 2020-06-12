# Budowanie projektu

Żeby utworzyć plik projektu naszego ulubionego edytora potrzebny nam będzie CMake w wersji co najmniej 3.10

W folderze głównym tworzymy folder o nazwie "build", a następnie uruchamiamy w nim konsolę. Projekt tworzymy komendą "CMake ../".

Mając stworzony projekt możemy skompilować pliki korzystając z naszego środowiska, albo wywołując komendę "CMake --build ."

Po skompilowaniu projektu trzeba przekopiować foldery "/Shaders" oraz "/Resources" oraz plik freetype.dll z folderu "/Library/vendor/freetype/win64"  do folderu "/build/Application/<Debug/Release>". Aplikacja nie zadziała poprawnie bez shaderów nich.
