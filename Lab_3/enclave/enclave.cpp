#include "enclave_t.h"
#include "sgx_trts.h"
#include <string.h>


const char table[7][41] = {
	"2BC7F22C0EF28F5C14DB4F2D4430AAD222532280",
	"5373FEDA0C3B53A1B425FFB8542E8CEAB8B8620F",
	"9334D8E51ADDF87674B4980600DA1060D62F0D4A",
	"2510A8AC5824C6873386B59923227BA9FFA942C1",
	"5660D3FEE4AC52F18BFF3CA31DDB5EF46A46D381",
	"4B368C460AB00A9D7CF4C35775D8CE8086F9AA6F",
	"BFF3CA31DDB5EF46A46D3812510A8AC5824C6873"
};

void foo(char* buf, size_t len, size_t idx) {
	if (idx < 7) {
		const char* data_ptr = data_ptr = table[idx];
		memcpy(buf, data_ptr, strlen(data_ptr + 1));
	}
	else {
		memset(buf, 0, strlen(table[0]));
	}
	return;
}
