# Temperature & Humidity Monitor

Author: John DiBaggio

Built with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

Using some utility code for C++ from [CPP Utils](https://github.com/nkolban/esp32-snippets).

## Make Setup
1. Run `make menuconfig`
	i. TODO
	ii. TODO
2. Run `make -j5`


## Eclipse IDE Setup
1. Open up the project properties

2. Expand C/C++ Build menu 

	i. Uncheck "Use default build command"
	ii. Go to Environment, and add the following environment variables:

		* IDF_PATH: `/Users/johndibaggio/esp/esp-idf`
		* PYTHON_PATH: `/Library/Python/2.7/site-packages`
		* PATH: ${PATH}:`/bin:/usr/bin:/usr/sbin:/sbin:/Users/johndibaggio/esp/xtensa-esp32-elf/bin`
		* BATCH_BUILD: `1`

3. Expand C/C++ General menu
	
	i. Go to Paths and Symbols
	
		* Import includes from [c_includes.xml](eclipse/c_includex.xml) file
	
	ii. Go to Preprocessor Include Paths, Macros, etc.
		
		1. Select the Providers tab
		2. Check the box for "CDT GCC Built-in Compiler Settings"
		3. Set the compiler spec command to `xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"`
4. Click Apply and Close
5. Rebuild the index


*Code in this repository is in the Public Domain (or CC0 licensed, at your option.)
Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.*
