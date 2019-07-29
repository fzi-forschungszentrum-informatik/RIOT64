PKG_NAME=hacl
PKG_URL=https://github.com/project-everest/hacl-star
PKG_VERSION=9dff786a3723baa393adb32ec258091ba056ba1b
PKG_LICENSE=Apache-2.0
PKG_FILTER=snapshots/hacl-c


.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
	

include $(RIOTBASE)/pkg/pkg.mk
