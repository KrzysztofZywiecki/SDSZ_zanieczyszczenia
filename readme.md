# Budowanie projektu

Żeby utworzyć plik projektu naszego ulubionego edytora potrzebny nam będzie CMake w wersji co najmniej 3.10

W folderze głównym tworzymy folder o nazwie "build", a następnie uruchamiamy w nim konsolę. Projekt tworzymy komendą "CMake ../".

Mając stworzony projekt możemy skompilować pliki korzystając z naszego środowiska, albo wywołując komendę "CMake --build ."

Po skompilowaniu projektu trzeba przekopiować folder "/Shaders" do folderu "/build/Application/Debug". Aplikacja nie zadziała poprawnie bez shaderów.

#Notki co do edytowania

Pliki możemy edytować korzystając z utworzonego projektu. Zapisane zmiany zostaną wpisane do edytowanych plików.
Natomiast przy utworzeniu nowego pliku ten będzie tworzony w folderze "build" (nie to co chcemy). Trzeba go wtedy ręcznie przenosić do odpowiedniego folderu w /Application albo /Library.

Zalecanym edytorem jest VS Code z otwartym folderem głównym, ale zabieramy sobie wtedy możliwość debugowania. Osobiście w takiej sytuacji otwieram plik projektu w Visual Studio i korzystam z jego debugera.
