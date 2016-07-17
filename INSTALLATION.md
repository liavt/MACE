#Installation

* Download the MACE source. 
	* This can be done via Git: Type `git clone https://github.com/liavt/mace.git` in terminal. On Windows, you will need MingW or a similar Unix emulator
	* You can also use  the Git GUI if you don't like command  line.
* Download CMake
	* On Windows systems, make sure to add it to your Path.
* Create a new folder called build in the MACE source directory
* Open the terminal.
	* On Windows systems, open the standard command prompt.
* Type `cd <path to where you downloaded the MACE source>`
	* On Windows, you may need to change to the drive via `cd \d  <drive letter>`
* Type `cd build`
* Finally, type `cmake ..`
	* You will need an internet connection for this step.
* You should now be installing MACE!
* If you are on Windows, a Visual Studio solution file should have been generated. Open it to find all of the source code linked and ready!
	* To build MACE, simply use Visual Studio to build the ALL-BUILD project
* If you are on OSX, you will have a choice of generating a Makefile or an XCode project
* If you are on Linux,  you will have a Makefile generated for you. Simply type `make <path to makefile>` to compile it via GCC
