bool phy_check_valid(int speed, int duplex, unsigned long *features){
	/* inline 'phy_caps_valid' here */
	int capa = 0;
	struct link_capabilities {
		int speed;
		unsigned int duplex;
		__ETHTOOL_DECLARE_LINK_MODE_MASK(linkmodes);
	}link_caps[__LINK_CAPA_MAX] = {
		{ SPEED_10, DUPLEX_HALF, {0} }, /* LINK_CAPA_10HD */
		{ SPEED_10, DUPLEX_FULL, {0} }, /* LINK_CAPA_10FD */
		{ SPEED_100, DUPLEX_HALF, {0} }, /* LINK_CAPA_100HD */
		{ SPEED_100, DUPLEX_FULL, {0} }, /* LINK_CAPA_100FD */
		{ SPEED_1000, DUPLEX_HALF, {0} }, /* LINK_CAPA_1000HD */
		{ SPEED_1000, DUPLEX_FULL, {0} }, /* LINK_CAPA_1000FD */
		{ SPEED_2500, DUPLEX_FULL, {0} }, /* LINK_CAPA_2500FD */
		{ SPEED_5000, DUPLEX_FULL, {0} }, /* LINK_CAPA_5000FD */
		{ SPEED_10000, DUPLEX_FULL, {0} }, /* LINK_CAPA_10000FD */
		{ SPEED_20000, DUPLEX_FULL, {0} }, /* LINK_CAPA_20000FD */
		{ SPEED_25000, DUPLEX_FULL, {0} }, /* LINK_CAPA_25000FD */
		{ SPEED_40000, DUPLEX_FULL, {0} }, /* LINK_CAPA_40000FD */
		{ SPEED_50000, DUPLEX_FULL, {0} }, /* LINK_CAPA_50000FD */
		{ SPEED_56000, DUPLEX_FULL, {0} }, /* LINK_CAPA_56000FD */
		{ SPEED_100000, DUPLEX_FULL, {0} }, /* LINK_CAPA_100000FD */
		{ SPEED_200000, DUPLEX_FULL, {0} }, /* LINK_CAPA_200000FD */
		{ SPEED_400000, DUPLEX_FULL, {0} }, /* LINK_CAPA_400000FD */
		{ SPEED_800000, DUPLEX_FULL, {0} }, /* LINK_CAPA_800000FD */
		{ SPEED_1600000, DUPLEX_FULL, {0} }, /* LINK_CAPA_1600000FD */
	};

	capa = speed_duplex_to_capa(speed, duplex);
	if (capa < 0){
		return false;
	}
	return linkmode_intersects(link_caps[capa].linkmodes, features);
}
