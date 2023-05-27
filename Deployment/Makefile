BINARYCREATOR=/opt/Qt/Tools/QtInstallerFramework/4.4/bin/binarycreator
DATA=packages/com.maskromtool.maskromtool/data

all: MaskRomToolInstaller

clean:
	rm -f MaskRomToolInstaller
	rm -rf $(DATA)


data:
	mkdir -p $(DATA)
	cd $(DATA) && cmake ../../../..
	cd $(DATA) && make -j 24

MaskRomToolInstaller: data
	$(BINARYCREATOR) -c config/config.xml -p packages -f MaskRomToolInstaller
