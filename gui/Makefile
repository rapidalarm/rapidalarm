.ONESHELL:

release:
	buildozer android release

genkey:
	keytool -genkey -v -keystore keystore/org-evanw-ventilator.keystore -alias ventilator -keyalg RSA -keysize 2048 -validity 10000

sign:
	jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore keystore/org-evanw-ventilator.keystore bin/myapp-0.1-armeabi-v7a-release-unsigned.apk ventilator
	~/.buildozer/android/platform/android-sdk/build-tools/30.0.0-rc2/zipalign -v 4 bin/myapp-0.1-armeabi-v7a-release-unsigned.apk bin/ventilator.apk

p4a:
	export ANDROIDSDK=/home/evan/Android/Sdk/
	export ANDROIDNDK=/home/evan/android/android-ndk-r21
	export ANDROIDAPI="27"
	export NDKAPI="21"
	ANDROIDNDKVER="r21"
	p4a apk --private ./ --package=org.evanw.ventilator --name "Vent" --version 0.1 --botstrap=sdl2 --requiements=python3,kivy,twisted
