include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=publish_mqtt
PKG_VERSION:=4.0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/publish_mqtt
  SECTION:=util
  CATEGORY:=Utilities
  TITLE:=Publish_mqtt
  DEPENDS:=+libmosquitto
endef

define Package/publish_mqtt/description
	Publish_mqtt example
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) Publish_mqtt/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/publish_mqtt/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/publish_mqtt $(1)/bin/
endef

$(eval $(call BuildPackage,publish_mqtt))
