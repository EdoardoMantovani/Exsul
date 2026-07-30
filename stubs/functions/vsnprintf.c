int vsnprintf(char *buf, size_t size, const char *fmt, va_list args){
	int ret = 0;
	int base = 0;
	int width = 0;
	int count = 0;
	int len_mod = 0;
	int precision = 0;
	bool left = false;
	bool plus = false;
	bool space = false;
	bool alt = false;
	bool zero = false;
	bool negative = false;
	bool upper = false;
	bool number = false;
	size_t pos = 0;
	char spec = 0;
	long long value = 0;
	const char *p = NULL;

	p = fmt != NULL ? fmt : "(null)";
	while(*p != 0){
		base = 10;
		value = 0;
		width = 0;
		precision = -1;
		len_mod = 0;
		spec = 0;

		if(*p != 37){
			if(buf != NULL && size != 0 && pos + 1 < size){
				buf[pos] = *p;
			}
			pos++;
			if(count < 2147483647){
				count++;
			}
			p++;
			continue;
		}

		p++;
		if(*p == 37){
			if(buf != NULL && size != 0 && pos + 1 < size){
				buf[pos] = 37;
			}
			pos++;
			if(count < 2147483647){
				count++;
			}
			p++;
			continue;
		}

		for(;;){
			if(*p == 45){
				left = true;
				p++;
			}else if(*p == 43){
				plus = true;
				p++;
			}else if(*p == 32){
				space = true;
				p++;
			}else if(*p == 35){
				alt = true;
				p++;
			}else if(*p == 48){
				zero = true;
				p++;
			}else{
				break;
			}
		}
		if(*p == 42){
			width = va_arg(args, int);
			if(width < 0){
				left = true;
				width = -width;
			}
			p++;
		}else{
			while(*p >= 48 && *p <= 57){
				width = width * 10 + (*p - 48);
				p++;
			}
		}

		if(*p == 46){
			p++;
			precision = 0;
			if(*p == 42){
				precision = va_arg(args, int);
				if(precision < 0){
					precision = -1;
				}
				p++;
			}else{
				while(*p >= 48 && *p <= 57){
					precision = precision * 10 + (*p - 48);
					p++;
				}
			}
		}

		if(*p == 104 && p[1] == 104){
			len_mod = 1;
			p += 2;
		}else if(*p == 104){
			len_mod = 2;
			p++;
		}else if(*p == 108 && p[1] == 108){
			len_mod = 4;
			p += 2;
		}else if(*p == 108){
			len_mod = 3;
			p++;
		}else if(*p == 122){
			len_mod = 5;
			p++;
		}else if(*p == 116){
			len_mod = 6;
			p++;
		}else if(*p == 106){
			len_mod = 7;
			p++;
		}

		spec = *p;
		if(spec == 0){
			if(buf != NULL && size != 0 && pos + 1 < size){
				buf[pos] = 37;
			}
			pos++;
			if(count < 2147483647){
				count++;
			}
			break;
		}
		p++;

		if(spec == 100 || spec == 105){
			long long signed_value = 0;
			switch(len_mod){
			case 1:
				signed_value = (signed char)va_arg(args, int);
				break;
			case 2:
				signed_value = (short)va_arg(args, int);
				break;
			case 3:
				signed_value = va_arg(args, long);
				break;
			case 4:
				signed_value = va_arg(args, long long);
				break;
			case 5:
				signed_value = (long long)va_arg(args, intptr_t);
				break;
			case 6:
				signed_value = (long long)va_arg(args, ptrdiff_t);
				break;
			case 7:
				signed_value = (long long)va_arg(args, intmax_t);
				break;
			default:
				signed_value = va_arg(args, int);
				break;
			}
			negative = signed_value < 0;
			value = (unsigned long long)signed_value;
			if(negative){
				value = 0 - value;
			}
			number = true;
			base = 10;
		}else if(spec == 117 || spec == 111 || spec == 120 || spec == 88){
			switch(len_mod){
			case 1:
				value = (unsigned char)va_arg(args, unsigned int);
				break;
			case 2:
				value = (unsigned short)va_arg(args, unsigned int);
				break;
			case 3:
				value = va_arg(args, unsigned long);
				break;
			case 4:
				value = va_arg(args, unsigned long long);
				break;
			case 5:
				value = (unsigned long long)va_arg(args, size_t);
				break;
			case 6:
				value = (unsigned long long)va_arg(args, uintptr_t);
				break;
			case 7:
				value = (unsigned long long)va_arg(args, uintmax_t);
				break;
			default:
				value = va_arg(args, unsigned int);
				break;
			}
			number = true;
			base = spec == 111 ? 8 : (spec == 117 ? 10 : 16);
			upper = spec == 88;
		}

	emit_number:
		if(number){
			char tmp[32];
			char prefix[2];
			char sign = 0;
			const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
			int prefix_len = 0;
			int len = 0;
			int zeroes = 0;
			int pad = 0;

			if(negative){
				sign = 45;
			}else if((spec == 100 || spec == 105) && plus){
				sign = 43;
			}else if((spec == 100 || spec == 105) && space){
				sign = 32;
			}

			if(value == 0 && precision == 0){
				len = 0;
			}else{
				do {
					tmp[len++] = digits[value % base];
					value /= base;
				} while(value != 0 && len < (int)sizeof(tmp));
			}

			if(alt){
				if(base == 16 && len != 0){
					prefix[0] = 48;
					prefix[1] = upper ? 88 : 120;
					prefix_len = 2;
				}else if(base == 8 && (len == 0 || tmp[len - 1] != 48)){
					prefix[0] = 48;
					prefix_len = 1;
				}
			}

			if(precision > len){
				zeroes = precision - len;
			}
			if(width > len + zeroes + prefix_len + (sign != 0)){
				pad = width - (len + zeroes + prefix_len + (sign != 0));
			}

			if(!left && !(zero && precision < 0)){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
				pad = 0;
			}
			if(sign != 0){
				if(buf != NULL && size != 0 && pos + 1 < size){
					buf[pos] = sign;
				}
				pos++;
				if(count < 2147483647){
					count++;
				}
			}
			for(int i = 0; i < prefix_len; i++){
				if(buf != NULL && size != 0 && pos + 1 < size){
					buf[pos] = prefix[i];
				}
				pos++;
				if(count < 2147483647){
					count++;
				}
			}
			if(!left && zero && precision < 0){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 48;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
				pad = 0;
			}
			while(zeroes-- > 0){
				if(buf != NULL && size != 0 && pos + 1 < size){
					buf[pos] = 48;
				}
				pos++;
				if(count < 2147483647){
					count++;
				}
			}
			while(len-- > 0){
				if(buf != NULL && size != 0 && pos + 1 < size){
					buf[pos] = tmp[len];
				}
				pos++;
				if(count < 2147483647){
					count++;
				}
			}
			if(left){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
			}
			continue;
		}

		if(spec == 99){
			char c = (char)va_arg(args, int);
			int pad = width > 1 ? width - 1 : 0;
			if(!left){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
				pad = 0;
			}
			if(buf != NULL && size != 0 && pos + 1 < size){
				buf[pos] = c;
			}
			pos++;
			if(count < 2147483647){
				count++;
			}
			if(left){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
			}
			continue;
		}

		if(spec == 115){
			const char *s = va_arg(args, const char *);
			int len = 0;
			int pad = 0;
			if(s == NULL){
				s = "(null)";
			}
			while(s[len] != 0 && (precision < 0 || len < precision)){
				len++;
			}
			if(width > len){
				pad = width - len;
			}
			if(!left){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
				pad = 0;
			}
			for(int i = 0; i < len; i++){
				if(buf != NULL && size != 0 && pos + 1 < size){
					buf[pos] = s[i];
				}
				pos++;
				if(count < 2147483647){
					count++;
				}
			}
			if(left){
				while(pad-- > 0){
					if(buf != NULL && size != 0 && pos + 1 < size){
						buf[pos] = 32;
					}
					pos++;
					if(count < 2147483647){
						count++;
					}
				}
			}
			continue;
		}

		if(spec == 112){
			const void *ptr = va_arg(args, const void *);
			const unsigned char *bytes = (const unsigned char *)ptr;
			const char *digits = "0123456789abcdef";

			if(*p == 77 || *p == 109){
				bool colon = *p == 77;
				p++;
				if(bytes == NULL){
					const char *s = "(null)";
					for(int i = 0; s[i] != 0; i++){
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = s[i];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}else{
					for(int i = 0; i < 6; i++){
						if(i != 0 && colon){
							if(buf != NULL && size != 0 && pos + 1 < size){
								buf[pos] = 58;
							}
							pos++;
							if(count < 2147483647){
								count++;
							}
						}
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = digits[(bytes[i] >> 4) & 15];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = digits[bytes[i] & 15];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}
				continue;
			}

			if(*p == 73 && p[1] == 52){
				p += 2;
				if(bytes == NULL){
					const char *s = "(null)";
					for(int i = 0; s[i] != 0; i++){
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = s[i];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}else{
					for(int i = 0; i < 4; i++){
						unsigned int v = bytes[i];
						char tmp[3];
						int len = 0;
						if(i != 0){
							if(buf != NULL && size != 0 && pos + 1 < size){
								buf[pos] = 46;
							}
							pos++;
							if(count < 2147483647){
								count++;
							}
						}
						do {
							tmp[len++] = (char)(48 + (v % 10));
							v /= 10;
						} while(v != 0);
						while(len-- > 0){
							if(buf != NULL && size != 0 && pos + 1 < size){
								buf[pos] = tmp[len];
							}
							pos++;
							if(count < 2147483647){
								count++;
							}
						}
					}
				}
				continue;
			}

			if(*p == 104){
				char sep = 0;
				int n = width > 0 ? width : 1;
				p++;
				if(*p == 67){
					sep = 58;
					p++;
				}else if(*p == 68){
					sep = 45;
					p++;
				}else if(*p == 78){
					p++;
				}
				if(bytes == NULL){
					const char *s = "(null)";
					for(int i = 0; s[i] != 0; i++){
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = s[i];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}else{
					for(int i = 0; i < n; i++){
						if(i != 0 && sep != 0){
							if(buf != NULL && size != 0 && pos + 1 < size){
								buf[pos] = sep;
							}
							pos++;
							if(count < 2147483647){
								count++;
							}
						}
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = digits[(bytes[i] >> 4) & 15];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = digits[bytes[i] & 15];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}
				continue;
			}

			if(*p == 86){
				struct va_format *vf = (struct va_format *)ptr;
				char tmp[512];
				int len = 0;
				p++;
				if(vf != NULL && vf->fmt != NULL && vf->va != NULL){
					va_list copy;
					va_copy(copy, *vf->va);
					len = vsnprintf(tmp, sizeof(tmp), vf->fmt, copy);
					va_end(copy);
					if(len < 0){
						len = 0;
					}
					if(len >= (int)sizeof(tmp)){
						len = (int)sizeof(tmp) - 1;
					}
					for(int i = 0; i < len; i++){
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = tmp[i];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}else{
					const char *s = "(null)";
					for(int i = 0; s[i] != 0; i++){
						if(buf != NULL && size != 0 && pos + 1 < size){
							buf[pos] = s[i];
						}
						pos++;
						if(count < 2147483647){
							count++;
						}
					}
				}
				continue;
			}

			while((*p >= 48 && *p <= 57) || (*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122)){
				p++;
			}
			value = (unsigned long long)(uintptr_t)ptr;
			base = 16;
			alt = true;
			number = true;
			goto emit_number;
		}

		if(spec == 110){
			if(len_mod == 1){
				*va_arg(args, signed char *) = (signed char)count;
			}else if(len_mod == 2){
				*va_arg(args, short *) = (short)count;
			}else if(len_mod == 3){
				*va_arg(args, long *) = count;
			}else if(len_mod == 4){
				*va_arg(args, long long *) = count;
			}else{
				*va_arg(args, int *) = count;
			}
			continue;
		}

		if(buf != NULL && size != 0 && pos + 1 < size){
			buf[pos] = 37;
		}
		pos++;
		if(count < 2147483647){
			count++;
		}
		if(buf != NULL && size != 0 && pos + 1 < size){
			buf[pos] = spec;
		}
		pos++;
		if(count < 2147483647){
			count++;
		}
	}

	if(buf != NULL && size != 0){
		size_t end = pos < size ? pos : size - 1;
		buf[end] = 0;
	}

	ret = count;
	return ret;
}
