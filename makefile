make:
		@echo Compiling..		
		@g++ simulator.cpp -o terminal
		
clean:
		@rm -rf terminal
