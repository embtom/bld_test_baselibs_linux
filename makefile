
build_debug_dir := build/x86/Debug
build_release_dir := build/x86/Release

build_debug_make := $(build_debug_dir)/Makefile
build_release_make := $(build_release_dir)/Makefile

#############################################################
# Main build targets
#############################################################  

all: release_build debug_build

clean: debug_clean release_clean
	
distclean: release_distclean debug_distclean

#############################################################
# Sub build targets (Release)
#############################################################
release_build: $(build_release_make) 
	cmake --build $(dir $<)

release_clean: 
	make -C$(dir $(build_release_make)) clean
	
release_distclean:
	rm $(build_release_dir) -R 

$(build_release_make):
	mkdir -p $(@D)
	cd $(@D) && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../../..


#############################################################
# Sub build targets (Debug)
#############################################################
debug_build: $(build_debug_make) 
	cmake --build $(dir $<)

debug_clean: 
	make -C$(dir $(build_debug_make)) clean

debug_distclean:
	rm $(build_debug_dir) -R
	
$(build_debug_make):
	mkdir -p $(@D)
	cd $(@D) && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../../..
		






