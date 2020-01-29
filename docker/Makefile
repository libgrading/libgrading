BASE_GS_IMAGE=	gradescope/auto-builds
CLANG_IMAGE=	libgrading/gradescope-clang
LIB_IMAGE=	libgrading/gradescope-libgrading

all: libgrading-image
	@echo "Images build; run 'make push' to push to DockerHub"

push: all
	docker push $(CLANG_IMAGE)
	docker push $(LIB_IMAGE):base
	docker push $(LIB_IMAGE):clang

libgrading-image: build-libgrading clang-image
	docker build -t $(LIB_IMAGE):base --build-arg base=$(BASE_GS_IMAGE) libgrading
	docker build -t $(LIB_IMAGE):clang --build-arg base=$(CLANG_IMAGE) libgrading

build-libgrading:
	mkdir -p libgrading/build
	cd libgrading/build && cmake -D CMAKE_BUILD_TYPE=Release ../../..
	make -C libgrading/build

clang-image:
	docker build -t $(CLANG_IMAGE) gradescope-clang
