#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)
PKG_CLONE_DIR?=$(PKG_BUILDDIR)/.git-clone-dir

ifneq (,$(PKG_FILTER))
PKG_FILTER_DIR=$(PKG_CLONE_DIR)/$(PKG_FILTER)/
else
PKG_FILTER_DIR=$(PKG_CLONE_DIR)/
endif

PKG_FILTER_TARGET=$(PKG_BUILDDIR)/.git-filtered 

# allow overriding package source with local folder (useful during development)
ifneq (,$(PKG_SOURCE_LOCAL))
  include $(RIOTBASE)/pkg/local.mk
else

.PHONY: prepare git-download clean git-cache-add

git-cache-add:
	$(GITCACHE) add "$(PKG_URL)" "$(PKG_VERSION)"

prepare: git-download

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_FILTER_TARGET): $(PKG_BUILDDIR)/.git-patched
else
$(PKG_FILTER_TARGET): $(PKG_BUILDDIR)/.git-downloaded
endif
	mkdir -p "$(PKG_FILTER_DIR)"
	touch "$(PKG_FILTER_DIR)".git-filtered
	find "$(PKG_FILTER_DIR)" -mindepth 1 -maxdepth 1 -exec mv -t "$(PKG_BUILDDIR)" -- {} +
git-download: $(PKG_FILTER_TARGET)

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= --no-gpg-sign --ignore-whitespace

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: $(PKG_BUILDDIR)/.git-downloaded $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	git -C $(PKG_CLONE_DIR) checkout -f $(PKG_VERSION)
	git $(GITFLAGS) -C $(PKG_CLONE_DIR) am $(GITAMFLAGS) "$(PKG_DIR)"/patches/*.patch
	touch $@
endif

$(PKG_BUILDDIR)/.git-downloaded:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	mkdir $(PKG_CLONE_DIR)
	$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_CLONE_DIR)"
	touch $@

clean::
	@test -d $(PKG_BUILDDIR) && { \
		rm $(PKG_BUILDDIR)/.git-filtered ; \
		rm -rf "$(PKG_CLONE_DIR)" ; \
		rm $(PKG_BUILDDIR)/.git-patched ; \
		git -C $(PKG_BUILDDIR) clean -f ; \
		git -C $(PKG_BUILDDIR) checkout "$(PKG_VERSION)"; \
		make $(PKG_BUILDDIR)/.git-patched ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	rm -rf "$(PKG_BUILDDIR)"

endif
