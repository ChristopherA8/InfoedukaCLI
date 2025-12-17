all:
	@clang++ src/main.cpp -std=c++17 \
  	-I /opt/homebrew/include \
 	-L /opt/homebrew/lib \
  	-lftxui-screen \
  	-lftxui-dom \
  	-lftxui-component \
	-lcpr \
	-lcurl \
  	-o dist/main
	@clear
	@./dist/main

publish:
	@clang++ src/main.cpp -std=c++17 \
  	-I /opt/homebrew/include \
 	-L /opt/homebrew/lib \
  	-lftxui-screen \
  	-lftxui-dom \
  	-lftxui-component \
	-lcpr \
	-lcurl \
  	-o dist/main
	@clear
	@cp ./dist/main /opt/InfoedukaCLI/app