const struct element * cfg80211_find_elem_match(u8 eid, const u8 *ies, unsigned int len,  const u8 *match, unsigned int match_len, unsigned int match_offset){
	const struct element *elem = NULL;

	for_each_element_id(elem, eid, ies, len) {
		if (elem->datalen >= match_offset + match_len && !memcmp(elem->data + match_offset, match, match_len)){
			return elem;
		}
	}
	return NULL;
}
