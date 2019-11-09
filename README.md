# Temperature & Humidity Monitor

Author: John DiBaggio

Built with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

Using [CPP Utils](https://github.com/nkolban/esp32-snippets) and [DHT22](https://github.com/gosouth/DHT22-cpp).

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

## License

GNU General Public License v3.0

Permissions of this strong copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved. Contributors provide an express grant of patent rights.
