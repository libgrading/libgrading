ifdef NINJA

BUILD_TOOL=	ninja
GENERATOR=	Ninja

else

BUILD_TOOL=	make
GENERATOR=	Unix Makefiles

endif

BASE_GS_IMAGE=	gradescope/auto-builds
CLANG_IMAGE=	libgrading/gradescope-clang
LIB_IMAGE=	libgrading/gradescope-libgrading

all: do-build

do-build:
	mkdir -p build
	cmake -B build -G "$(GENERATOR)" .
	$(BUILD_TOOL) -C build

docker:
	docker build -t $(CLANG_IMAGE) -f Dockerfile.base .
	docker build -t $(LIB_IMAGE):base --build-arg base=$(BASE_GS_IMAGE) .
	docker build -t $(LIB_IMAGE):clang --build-arg base=$(CLANG_IMAGE) .
	@echo "Images build; run 'make upload' to push to DockerHub"

upload:
	docker push $(CLANG_IMAGE)
	docker push $(LIB_IMAGE):base
	docker push $(LIB_IMAGE):clang

clean:
	rm -r build
