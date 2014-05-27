#include "strres.h"
#include "cocos2d.h"

USING_NS_CC;

StrRes* s_SharedInstance = nullptr;

int StrRes::loadStrings(char * path, int lang) {
	char supported[][8] = {"en_US", "zh_CN"};
	FileUtils* fu = FileUtils::getInstance();
	int index = 0;
	int i, j;
	cstr p;
	char last;
	ssize_t fileSize;
	LanguageType t;
	if (lang < 0) {
		t = Application::getInstance()->getCurrentLanguage();
		switch (t) {
		case LanguageType::ENGLISH:
			index = 0;
			break;
		case LanguageType::CHINESE:
			index = 1;
			break;
		default:
			index = 0;
		}
	}
	else if (lang > 1) {
		index = 0;
	}
	else {
		index = lang;
	}
	if (this->_buffer != NULL) {
		delete this->_buffer;
		delete this->_ptrs;
	}
	std::string name = "strings/";
	std::string fullpath = fu->fullPathForFilename(name + supported[index]);
	this->_buffer = (char *)fu->getFileData(fullpath, "rb", &fileSize);
	this->_ptrs = (cstr *)malloc(sizeof(cstr)* RSTR::_not_used);
	p = this->_buffer;
	last = 0;
	for (i = j = 0; i < fileSize; ++i) {
		if (last == 0) {
			this->_ptrs[j++] = p;
		}
		last = *p;
		p++;
	}
	return 0;
}

char * StrRes::getString(int id) {
	assert(id >= 0 && id < RSTR::_not_used);
	return this->_ptrs[id];
}

StrRes * StrRes::getInstance() {
	if (s_SharedInstance == NULL){
		s_SharedInstance = new StrRes();
		s_SharedInstance->loadStrings();
	}
	return s_SharedInstance;
}

void StrRes::killInstance(StrRes *instance) {
	if (s_SharedInstance != nullptr) {
		delete s_SharedInstance;
		s_SharedInstance = nullptr;
	}
}