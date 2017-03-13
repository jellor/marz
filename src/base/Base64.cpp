#include <limits>
#include "Base64.h"

namespace marz {

const char Base64::encoding_table[64] =
	{
	/* 	 0    1	   2    3	 4	  5    6    7	*/
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	/* 	 8    9	  10   11   12   13   14   15   */
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	/* 	16   17   18   19   20   21   22   23   */
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	/* 	24   25	  26   27   28   29   30   31   */
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	/* 	32   33   34   35   36   37   38   39   */
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	/* 	40   41	  42   43   44	 45   46   47   */
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	/* 	48   49	  50   51   52   53   54   55   */
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
	/* 	56   57   58   59   60   61   62   63   */
		'4', '5', '6', '7', '8', '9', '+', '/',
	};

const char Base64::decoding_table[128] =
	{
	/*null	   soh	   stx	   etx     eot     enq     ack     bel	 */
    	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /*  bs      ht      nl      vt      ff      cr      so      si   */
    	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /* dle     dc1     dc2     dc3     dc4     nak     syn     etb   */
    	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /* can      em     sub     esc      fs      gs      re      us   */
    	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /*  sp       !       "       #       $       %       &       `   */
    	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /*   (       )       *       +       ,       -       .       /   */
    	-1, 	-1, 	-1, 	62, 	-1, 	-1, 	-1, 	63,
    /*   0       1       2       3       4       5       6       7   */
    	52, 	53, 	54, 	55, 	56, 	57, 	58, 	59,
    /*   8       9       :       ;       <       =       >       ?   */
    	60, 	61, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1,
    /*   @       A       B       C       D       E       F       G   */
    	-1,  	 0,  	 1,  	 2,  	 3,  	 4,  	 5,  	 6,
    /*   H       I       J       K       L       M       N       O   */
    	 7,  	 8,  	 9, 	10, 	11, 	12, 	13, 	14,
    /*   P       Q       R       S       T       U       V       W   */
    	15, 	16, 	17, 	18, 	19, 	20, 	21, 	22,
    /*   X       Y       Z       [       \       ]       ^       _   */
    	23, 	24, 	25, 	-1, 	-1, 	-1, 	-1, 	-1,
    /*   '       a       b       c       d       e       f       g   */
    	-1, 	26, 	27, 	28, 	29, 	30, 	31, 	32,
    /*   h       i       j       k       l       m       n       o   */
    	33, 	34, 	35, 	36, 	37, 	38, 	39, 	40,
    /*   p       q       r       s       t       u       v       w   */
    	41, 	42, 	43, 	44, 	45, 	46, 	47, 	48,
    /*   x       y       z       {       |       }       ~     del   */
    	49, 	50, 	51, 	-1, 	-1, 	-1, 	-1, 	-1,
	};

std::string Base64::Encode(const std::string& binary_data) {
	int str_length = binary_data.size();

	if (str_length > (std::numeric_limits<std::string::size_type>::max() / 4u) * 3u) {
		return "";
	}

	std::string ascii_data(((str_length + 2) / 3) * 4, '=');

	int i  = 0;
	int it = 0;
	char c1, c2, c3;

	while (it < str_length) {

		c1 = binary_data[it ++] & 0xFF;
		if (it >= str_length) {
			ascii_data[i ++] = encoding_table[(c1 >> 2) & 0x3F];
			ascii_data[i ++] = encoding_table[(c1 << 4) & 0x30];
			ascii_data[i ++] = '=';
			ascii_data[i ++] = '=';
			break;
		}

		c2 = binary_data[it ++] & 0xFF;
		if (it >= str_length) {
			ascii_data[i ++] = encoding_table[(c1 >> 2) & 0x3F];
			ascii_data[i ++] = encoding_table[((c1 << 4) & 0x30) | ((c2 >> 4) & 0x0F)];
			ascii_data[i ++] = encoding_table[(c2 << 2) & 0x3C];
			ascii_data[i ++] = '=';
			break;
		}

		c3 = binary_data[it ++] & 0xFF;
		ascii_data[i ++] = encoding_table[(c1 >> 2) & 0x3F];
		ascii_data[i ++] = encoding_table[((c1 << 4) & 0x30) | ((c2 >> 4) & 0x0F)];
		ascii_data[i ++] = encoding_table[((c2 << 2) & 0x3C) | ((c3 >> 6) & 0x03)];
		ascii_data[i ++] = encoding_table[c3 & 0x3F];

	}

	return ascii_data;
}

std::string Base64::Decode(const std::string& ascii_data) {
	int str_length = ascii_data.size();

	std::string binary_data;

	int it = 0;
	int flag_i, flag_j;
	char c1, c2, c3, c4;

	while (it < str_length) {

		c1 = decoding_table[ascii_data[it ++]];
		if (c1 == -1) {
			return "";
		}

		c2 = decoding_table[ascii_data[it ++]];
		if (c2 == -1) {
			return "";
		}

		binary_data += ((c1 << 2) & 0xFC) | ((c2 >> 4) & 0x03);

		flag_i = ascii_data[it ++];
		if (flag_i == '=') {
			return binary_data;
		}

		c3 = decoding_table[flag_i];
		if (c3 == -1) {
			return "";
		} else {
			binary_data += ((c2 << 4) & 0xF0) | ((c3 >> 2) & 0x0F);
		}

		flag_j = ascii_data[it ++];
		if (flag_j == '=') {
			return binary_data;
		}

		c4 = decoding_table[flag_j];
		if (c4 == -1) {
			return "";
		} else {
			binary_data += ((c3 << 6) & 0xC0) | (c4 & 0x3F);
		}

	}

	return binary_data;
}

} // namespace marz
