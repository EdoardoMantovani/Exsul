/* Made by Edoardo Mantovani - 2026, header for the standalone ELF/PE API */

/* get_director: due to the fact that the global variable 'director' isn't accessible in the standalone ELF, I made a simple helper which returns it */
void *get_director(void){
	return &director;
}

/* get_ieee80211_local: return the pointer to the internal 'ieee80211_local' struct */
void *get_ieee80211_local(void){
	return director.ieee80211_local;
}

/* get_wiphy: return the current module's 'wiphy' struct */
void *get_wiphy(void){
	return director.wiphy;
}

/* get_wireless_dev: return the current module's 'wireless_dev' struct */
void *get_wireless_dev(void){
	return director.wdev;
}

/* get_net_device: return the current module's 'net_device' struct */
void *get_net_device(void){
	return director.net_device;
}

/* get_ieee80211_hw: used in the standalone ELF api for getting the pointer of the struct 'ieee80211_hw' used in the module */
void *get_ieee80211_hw(void){
	if(director.ieee80211_local == NULL){
		return NULL;
	}
	return director.ieee80211_local->ieee80211_hw;
}

/* get_ieee80211_priv_data: get the 'ieee80211_hw->priv' ptr */
void *get_ieee80211_priv_data(void){
	void *hw = NULL;

	hw = get_ieee80211_hw();
	if(hw == NULL){
		return NULL;
	}
	return ieee80211_priv(hw);
}

/* skb_get_data: API function for the standalone ELF which permits to get the 'data' field fo the 'skb', not possible from the tcc scripting environment */
void *skb_get_data(struct sk_buff *skb){
	if(skb == NULL){
		return NULL;
	}
	return skb->data;
}

/* wifi_scan_stop: stop periodic scans and let the active hardware scan finish */
int wifi_scan_stop(void){
	int ret = 0;

	stop_wifi_scan_thread();
	scan_process_deferred();
	if (!atomic_load(&director.scan_in_progress)){
		return ret;
	}
	ret = connection_leave_idle(director.ieee80211_local);
	for (int i = 0; i < 160 && atomic_load(&director.scan_in_progress); i++){
		usleep(100000);
		scan_process_deferred();
	}
	ret = atomic_load(&director.scan_in_progress) ? (ret ? ret : -EBUSY) : 0;
	return ret;
}

/* wifi_connect_network: high level API for connecting to a Wi-Fi network */
int wifi_connect(unsigned char *network_name, unsigned char *network_passkey){
	int ret = 0;
	struct ieee80211_local *local = NULL;

	if(network_name == NULL || network_passkey == NULL || strlen(network_name) <= 1 || strlen(network_passkey) <= 8){
		ret = -EINVAL;
		return ret;
	}
	local = get_ieee80211_local();
	if(local == NULL){
		ret = -EINVAL;
		return ret;
	}
	/* ignore the return value of the 'wifi_scan_stop' function */
	ret = wifi_scan_stop();
	ret = wifi_auth_connection_start(local, network_name, network_passkey);
	return ret;
}

/* wifi_start_ap: high level API for starting a Wi-Fi network */
int wifi_start_ap(unsigned char *network_ssid, unsigned char *network_passkey){
	int ret = 0;
	unsigned char *argv[] = {NULL, NULL, NULL, NULL};

	if(network_ssid == NULL || network_passkey == NULL || strlen(network_ssid) <= 1 || strlen(network_passkey) <= 8){
		ret = -EINVAL;
		return ret;
	}
	argv[0] = "ops";
	argv[1] = "start_ap";
	argv[2] = network_ssid;
	argv[3] = network_passkey;
	ret = drv_cmd_ops(&director, argv, 4);
	return ret;
}

/* wifi_start_monitor: high level API for starting monitor mode */
int wifi_start_monitor(void){
	int ret = 0;
	unsigned char *argv[] = {NULL, NULL};

	argv[0] = "ops";
	argv[1] = "monitor";
	ret = drv_cmd_ops(&director, argv, 2);
	return ret;
}

/* wifi_suspend: high level API for suspending the Wi-Fi firmware, useful for entering in WoWlan mode */
int wifi_suspend(void){
	int ret = 0;
	unsigned char *argv[] = {NULL, NULL};

	argv[0] = "ops";
	argv[1] = "suspend";
	ret = drv_cmd_ops(&director, argv, 2);
	return ret;
}

/* wifi_scan_once: start one hardware scan and wait for its completion callback */
int wifi_scan_once(unsigned int timeout_ms){
	int ret = 0;
	unsigned int sleep_ms = 0;
	unsigned int elapsed_ms = 0;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_local *local = NULL;

	ops = director.ieee80211_ops;
	local = director.ieee80211_local;

	if (local == NULL || local->ieee80211_hw == NULL || local->ieee80211_vif == NULL || ops == NULL || ops->hw_scan == NULL || timeout_ms == 0){
		ret = -EINVAL;
		return ret;
	}
	if (atomic_load(&director.scan_in_progress)){
		ret = -EBUSY;
		return ret;
	}
	ret = connection_leave_idle(local);
	if (ret){
		return ret;
	}
	ret = start_hw_scan(local, ops->hw_scan, ops->cancel_hw_scan);
	if (ret){
		return ret;
	}
	while (atomic_load(&director.scan_in_progress) && elapsed_ms < timeout_ms){
		sleep_ms = timeout_ms - elapsed_ms;

		if (sleep_ms > 100){
			sleep_ms = 100;
		}
		usleep(sleep_ms * 1000);
		elapsed_ms += sleep_ms;
		scan_process_deferred();
	}
	if (atomic_load(&director.scan_in_progress)){
		ret = -ETIMEDOUT;
		return ret;
	}
	ret = test_bit(SCAN_ABORTED, &local->scanning) ? -ECANCELED : 0;
	return ret;
}

/* elf_reset_usb: reset the usb device with 'libusb_reset_device' */
int elf_reset_usb(void){
	int ret = 0;

	if(director.usb_dev != NULL){
		ret = libusb_reset_device(director.usb_dev);
	}else{
		ret = -EINVAL;
	}
	return ret;
}

/* usb_assign_rx_observer: Passive usb observation for runtime tools such as the CSI capture, it is very similar to its fuzzing counterpart */
int usb_assign_rx_observer(int (*callback)(void *data, int len)){
	int ret = 0;

	if(director.usb_rx_observer_initialized == false){
		if(callback == NULL){
			return ret;
		}else{
			ret = -ENODEV;
			return ret;
		}
	}
	pthread_mutex_lock(&director.usb_rx_observer_lock);
	if(callback != NULL && director.usb_rx_observer != NULL){
		ret = -EBUSY;
	}else{
		director.usb_rx_observer = callback;
		if(callback == NULL){
			while(director.usb_rx_observer_refs != 0){
				pthread_cond_wait(&director.usb_rx_observer_idle, &director.usb_rx_observer_lock);
			}
		}
	}
	pthread_mutex_unlock(&director.usb_rx_observer_lock);
	return ret;
}

/* usb_rx_observer_destroy: deinitialize the usb rx observer callback */
void usb_rx_observer_destroy(void){
	if(director.usb_rx_observer_initialized == false){
		return;
	}
	usb_assign_rx_observer(NULL);
	pthread_cond_destroy(&director.usb_rx_observer_idle);
	pthread_mutex_destroy(&director.usb_rx_observer_lock);
	director.usb_rx_observer_initialized = false;
}

/* wifi_mesh_join: given the mesh name and its frequency, search and join the mesh (if exists) */
int wifi_mesh_join(const char *mesh_id, unsigned int frequency){
	int ret = 0;
	struct cfg80211_chan_def *chandef = NULL;
	struct ieee80211_channel *channel = NULL;
	struct ieee80211_vif*ieee80211_vif = NULL;
	struct mesh_setup setup = {};

	if (mesh_id == NULL || strlen(mesh_id) == 0 || strlen(mesh_id) > IEEE80211_MAX_MESH_ID_LEN || director.wiphy == NULL){
		printf("[!] invalid mesh_id!\n");
		ret = -EINVAL;
		return ret;
	}
	channel = connection_find_channel(director.wiphy, frequency);
	if (channel == NULL || director.struct_sizes.cfg80211_chan_def == 0){
		printf("[!] invalid ieee80211_channel\n");
		ret = -EINVAL;
		return ret;
	}
	chandef = calloc(1, director.struct_sizes.cfg80211_chan_def);
	if (chandef == NULL){
		printf("[!] failed to allocate chandef!\n");
		ret = -ENOMEM;
		return ret;
	}
	cfg80211_chandef_create(chandef, channel, NL80211_CHAN_NO_HT);
	setup.chandef = chandef;
	setup.mesh_id = (const u8 *)mesh_id;
	setup.mesh_id_len = strlen(mesh_id);
	setup.path_sel_proto = IEEE80211_PATH_PROTOCOL_HWMP;
	setup.path_metric = IEEE80211_PATH_METRIC_AIRTIME;
	setup.sync_method = IEEE80211_SYNC_METHOD_NEIGHBOR_OFFSET;
	setup.beacon_interval = 100;
	setup.dtim_period = 2;
	ret = ieee80211_join_mesh(director.wiphy, NULL, &default_mesh_config, &setup);
	if(ret == 0 && director.peer_runtime->vif != NULL){
		ieee80211_vif = director.peer_runtime->vif;
		#ifdef ENABLE_DBG_PRINT
		printf("[*] mesh interface address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			ieee80211_vif->addr[0], ieee80211_vif->addr[1], ieee80211_vif->addr[2],
			ieee80211_vif->addr[3], ieee80211_vif->addr[4], ieee80211_vif->addr[5]);
		#endif
	}
	free(chandef);
	return ret;
}

/* wifi_mesh_leave: if we are associated in a mesh, leave it */
int wifi_mesh_leave(void){
	int ret = 0;

	pthread_mutex_lock(&director.peer_control_lock);
	if(director.peer_runtime->mesh_mode == true){
		ret = peer_stop_locked();
	}else{
		ret = -ENOTCONN;
	}
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

/* wifi_mesh_send: from the current mesh, send a payload to a specific mesh node */
int wifi_mesh_send(const u8 *destination, u16 proto, const void *payload, int len){
	int ret = 0;
	int hdrlen = 0;
	u8 *p = NULL;
	u8 *mesh = NULL;
	struct sk_buff *skb = NULL;
	struct peer_runtime *r = NULL;
	struct ieee80211_hw *hw = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;

	r = director.peer_runtime;
	if (!destination || len < 0 || (!payload && len) || len > 2304 || proto < 0x600 || is_zero_ether_addr(destination)){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (!atomic_load(&r->active) || !r->mesh_mode){
		ret = -ENOTCONN;
		goto out;
	}
	hw = r->local->ieee80211_hw;
	ieee80211_vif = r->vif;
	hdrlen = is_multicast_ether_addr(destination) ? 26 : 32;
	skb = dev_alloc_skb(hw->extra_tx_headroom + hdrlen + 14 + len);
	if (!skb){
		ret = -ENOMEM;
		goto out;
	}
	skb_reserve(skb, hw->extra_tx_headroom);
	p = skb_put_zero(skb, hdrlen + 14 + len);
	put_unaligned_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA | IEEE80211_FCTL_FROMDS | (hdrlen == 32 ? IEEE80211_FCTL_TODS : 0), p);
	memcpy(p + 4, destination, ETH_ALEN);
	memcpy(p + 10, ieee80211_vif->addr, ETH_ALEN);
	memcpy(p + 16, hdrlen == 32 ? destination : ieee80211_vif->addr, ETH_ALEN);
	if (hdrlen == 32){
		memcpy(p + 24, ieee80211_vif->addr, ETH_ALEN);
	}
	put_unaligned_le16((atomic_fetch_add(&r->sequence, 1) & 0xfff) << 4, p + 22);
	p[hdrlen - 1] = 1; /* QoS Mesh Control Present */
	mesh = p + hdrlen;
	mesh[1] = r->sdata->u.mesh.mshcfg.dot11MeshTTL;
	put_unaligned_le32(__atomic_add_fetch(&r->sdata->u.mesh.mesh_seqnum.counter, 1, __ATOMIC_RELAXED), mesh + 2);
	memcpy(mesh + 6, "\xaa\xaa\x03\x00\x00\x00", 6);
	mesh[12] = proto >> 8;
	mesh[13] = proto;
	if (len){
		memcpy(mesh + 14, payload, len);
	}
	ret = hdrlen == 26 ? peer_tx(skb, NULL, false) : mesh_queue_frame(skb, destination);
	out:
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

/* peer_receive: set the receive handler when connected to a mesh, permits to retrieve the data sent from 'wifi_mesh_send' */
int peer_receive(void *buffer, int capacity, unsigned int timeout_ms, bool mesh){
	int ret = 0;
	u64 deadline = 0;
	struct sk_buff *skb = NULL;

	deadline = connection_now_ms() + timeout_ms;
	if (buffer == NULL || capacity <= 0){
		ret = -EINVAL;
		return ret;
	}

	for (;;){
		pthread_mutex_lock(&director.peer_control_lock);
		pthread_mutex_lock(&director.peer_state_lock);
		if (!atomic_load(&director.peer_runtime->active) || director.peer_runtime->mesh_mode != mesh){
			ret = -ENOTCONN;
		}else if (director.peer_runtime->delivery_count){
			skb = director.peer_runtime->delivery[director.peer_runtime->delivery_head];
			ret = skb->len > capacity ? -EMSGSIZE : skb->len;
			if (ret >= 0){
				memcpy(buffer, skb->data, ret);
				director.peer_runtime->delivery[director.peer_runtime->delivery_head] = NULL;
				director.peer_runtime->delivery_head = (director.peer_runtime->delivery_head + 1) % PEER_QUEUE;
				director.peer_runtime->delivery_count--;
				kfree_skb(skb);
			}
		}else{
			ret = -EAGAIN;
		}
		pthread_mutex_unlock(&director.peer_state_lock);
		pthread_mutex_unlock(&director.peer_control_lock);
		if (ret != -EAGAIN || connection_now_ms() >= deadline){
			return ret;
		}
		usleep(10000);
	}
}

/* wifi_mesh_receive: wrapper of 'peer_receive' */
int wifi_mesh_receive(void *buffer, int capacity, unsigned int timeout_ms){
	int ret = 0;

	ret = peer_receive(buffer, capacity, timeout_ms, true);
	return ret;
}

/* wifi_mesh_get_peer: given the peer index, return the mac address of the peer present in the peer table */
int wifi_mesh_get_peer(unsigned int index, u8 *address, unsigned int *plink_state){
	int ret = 0;

	ret = -ENOENT;
	if (index >= MESH_PEERS || address == NULL || plink_state == NULL){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (director.peer_runtime->mesh_mode && director.peer_runtime->peers[index]){
		memcpy(address, director.peer_runtime->peers[index]->sta->addr, ETH_ALEN);
		*plink_state = director.peer_runtime->peers[index]->mesh.plink_state;
		ret = 0;
	}
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

/* wifi_p2p_receive: wrapper of 'peer_receive' for p2p */
int wifi_p2p_receive(void *buffer, int capacity, unsigned int timeout_ms){
	int ret = 0;

        ret = peer_receive(buffer, capacity, timeout_ms, false);
        return ret;
}

/* wifi_p2p_start: start the p2p (Wi-Fi direct) mode */
int wifi_p2p_start(const char *device_name, unsigned int listen_frequency){
	int ret = 0;

	if (device_name == NULL || strlen(device_name) == 0 || strlen(device_name) > 32 || (listen_frequency != 2412 && listen_frequency != 2437 && listen_frequency != 2462)){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&director.peer_control_lock);
	ret = peer_open(false, false, listen_frequency);
	if (ret == 0){
		memcpy(director.peer_runtime->device_name, device_name, strlen(device_name) + 1);
		director.peer_runtime->listen_frequency = listen_frequency;
		ret = peer_run();
		if (ret){
			peer_stop_locked();
		}
	}
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

/* wifi_p2p_stop: stop p2p mode, not tested */
int wifi_p2p_stop(void){
	int ret = 0;

	pthread_mutex_lock(&director.peer_control_lock);
	if(director.peer_runtime->mesh_mode == true){
		ret = -EBUSY;
	}else{
		ret = peer_stop_locked();
	}
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

/* wifi_p2p_listen: listen to any incoming p2p peers, still not tested */
int wifi_p2p_listen(unsigned int frequency, unsigned int duration_ms){
	int ret = 0;

	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (!atomic_load(&director.peer_runtime->active) || director.peer_runtime->mesh_mode || director.peer_runtime->p2p_go || director.peer_runtime->p2p_client){
		ret = -ENOTCONN;
	}else{
		director.peer_runtime->find_deadline = 0;
		ret = peer_roc_start(frequency, duration_ms);
	}
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

int wifi_p2p_find(unsigned int duration_ms){
	int ret = 0;

	if (!duration_ms || duration_ms > 3600000){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (!atomic_load(&director.peer_runtime->active) || director.peer_runtime->mesh_mode || director.peer_runtime->p2p_go || director.peer_runtime->p2p_client){
		ret = -ENOTCONN;
	}else{
		director.peer_runtime->social_index = 0;
		director.peer_runtime->beacon_at = 0;
		ret = peer_roc_start(2412, 300);
		if (ret == 0){
			director.peer_runtime->find_deadline = connection_now_ms() + duration_ms;
		}
	}
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

int wifi_p2p_get_peer(unsigned int index, struct p2p_peer *peer){
	int ret = 0;

	ret = -ENOENT;
	if (peer == NULL || index >= MESH_PEERS){
		ret = -EINVAL;
		return ret;
	}
	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (!director.peer_runtime->mesh_mode && director.peer_runtime->p2p_peers[index].last_seen_ms){
		*peer = director.peer_runtime->p2p_peers[index];
		ret = 0;
	}
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

int wifi_p2p_send_action(const u8 *destination, u8 subtype, u8 dialog_token, const void *attributes, int length){
	int ret = 0;
	int n = 0;
	u8 *p = NULL;
	u8 *attrs = NULL;
	struct sk_buff *skb = NULL;

	attrs = attributes;
	if (destination == NULL || is_multicast_ether_addr(destination) || is_zero_ether_addr(destination) || dialog_token == 0 || subtype > 8 || length < 0 || (!attributes && length) || length > 1024){
		ret = -EINVAL;
		return ret;
	}
	for (int off = 0; off < length;){
		if (length - off < 3 || wifi_wire_le16(attrs + off + 1) > length - off - 3 || wifi_wire_le16(attrs + off + 1) > 248){
			ret = -EINVAL;
			return ret;
		}
		off += 3 + wifi_wire_le16(attrs + off + 1);
	}
	pthread_mutex_lock(&director.peer_control_lock);
	pthread_mutex_lock(&director.peer_state_lock);
	if (!atomic_load(&director.peer_runtime->active) || director.peer_runtime->mesh_mode || (!director.peer_runtime->p2p_go && !director.peer_runtime->p2p_client && !atomic_load(&director.peer_runtime->roc_ready))){
		ret = -ENOTCONN;
		goto out;
	}
	skb = peer_frame(IEEE80211_STYPE_ACTION, destination, 8 + length * 3 + 6);
	if (skb == NULL){
		ret = -ENOMEM;
		goto out;
	}
	p = skb_put(skb, 8);
	memcpy(p, "\x04\x09\x50\x6f\x9a\x09", 6);
	p[6] = subtype;
	p[7] = dialog_token;
	for (int off = 0; off < length;){
		n = 3 + wifi_wire_le16(attrs + off + 1);
		p = skb_put(skb, 6 + n);
		p[0] = WLAN_EID_VENDOR_SPECIFIC;
		p[1] = 4 + n;
		memcpy(p + 2, "\x50\x6f\x9a\x09", 4);
		memcpy(p + 6, attrs + off, n);
		off += n;
	}
	ret = peer_tx(skb, NULL, !director.peer_runtime->p2p_go && !director.peer_runtime->p2p_client);
	out:
	pthread_mutex_unlock(&director.peer_state_lock);
	pthread_mutex_unlock(&director.peer_control_lock);
	return ret;
}

int wifi_mesh_join_config(const struct mesh_config *conf, const struct mesh_setup *setup){
	int ret = 0;

	ret = peer_mesh_join_config(conf, setup);
	return ret;
}

int wifi_mesh_get_path(const u8 *destination, u8 *next_hop, struct mpath_info *info){
	int ret = 0;

	ret =  peer_mesh_get_path(destination, next_hop, info);
	return ret;
}

int wifi_p2p_mgmt_tx(const void *frame, int length){
	int ret = 0;

	ret = peer_p2p_mgmt_tx(frame, length);
	return ret;
}

int wifi_p2p_group_connect(const char *ssid, const char *passphrase){
	int ret = 0;

	ret = peer_p2p_group_connect(ssid, passphrase);
	return ret;
}

int wifi_p2p_group_wait_connected(unsigned int timeout_ms){
	int ret = 0;

	ret = peer_p2p_group_wait_connected(timeout_ms);
	return ret;
}

int wifi_p2p_group_start(const char *ssid, unsigned int frequency, const u8 *ies, int ies_len){
	int ret = 0;

	ret =  peer_p2p_group_start(ssid, frequency, ies, ies_len);
	return ret;
}

/* wifi_p2p_sta_change: */
int wifi_p2p_sta_change(const u8 *address, u16 aid, enum ieee80211_sta_state state, const u8 *rates_ies, int length){
	int ret = 0;

	ret = peer_p2p_sta_change(address, aid, state, rates_ies, length);
	return ret;
}

int wifi_p2p_set_key(enum set_key_cmd command, const u8 *peer_address, struct ieee80211_key_conf *key){
	int ret = 0;

	ret = peer_p2p_set_key(command, peer_address, key);
	return ret;
}

/* hci_open_device: verify if an 'hci_dev' device is present in memory, verify its 'open' and 'send' callbacks and then open it with 'open' */
int hci_open_device(void *device){
	int ret = 0;
	struct hci_dev *hdev = NULL;
	int (*open)(struct hci_dev *hdev) = NULL;
	int (*send)(struct hci_dev *hdev, struct sk_buff *skb) = NULL;

	hdev = (struct hci_dev *)device;
	if(hdev == NULL){
		ret = -ENODEV;
		return ret;
	}
	open = hdev->open;
	send = hdev->send;
	if(open == NULL || send == NULL){
		ret = -ENODEV;
		return ret;
	}
	ret = test_bit(HCI_RUNNING, &hdev->flags);
	if(ret == true){
		set_bit(HCI_UP, &hdev->flags);
		return ret;
	}

	set_bit(HCI_INIT, &hdev->flags);
	ret = open(hdev);
	if(ret != 0){
		clear_bit(HCI_INIT, &hdev->flags);
		return ret;
	}
	set_bit(HCI_RUNNING, &hdev->flags);
	set_bit(HCI_UP, &hdev->flags);
	clear_bit(HCI_INIT, &hdev->flags);
	return ret;
}

/* hci_send_raw_command: intermediate function, create the actual HCI command and start the transmission */
int hci_send_raw_command(void *device, u16 opcode, u8 declared_length, const void *parameters, u32 wire_length){
	int ret = 0;
	struct sk_buff *skb = NULL;
	struct hci_dev *hdev = NULL;
	struct hci_command_hdr *header = NULL;

	hdev = (struct hci_dev *)device;
	if(hdev == NULL || wire_length > 0xff || (wire_length != 0 && parameters == NULL)){
		ret = -EINVAL;
		return ret;
	}
	ret = test_bit(HCI_RUNNING, &hdev->flags);
	if(ret == true){
		ret = -ENETDOWN;
		return ret;
	}
	skb = hci_cmd_sync_alloc(hdev, opcode, wire_length, parameters, NULL);
	if(skb == NULL){
		ret = -ENOMEM;
		return ret;
	}
	header = (struct hci_command_hdr *)skb->data;
	header->plen = declared_length;
	ret = hci_send_frame(hdev, skb);
	return ret;
}

/* hci_send_command: "high" level function for sending an HCI command to an opened 'hci_dev' */
int hci_send_command(void *device, u16 opcode, const void *parameters, u32 parameter_length){
	int ret = 0;

	if(parameter_length > 0xff){
		ret = -EINVAL;
		return ret;
	}
	ret = hci_send_raw_command(device, opcode, (u8)parameter_length, parameters, parameter_length);
}

/* hci_transport_lost: return 'true' if the HCI device isn't detected anymore by libusb, the state is usually determined after the libusb tx callback */
int hci_transport_lost(void){
	int ret = 0;

	ret = atomic_load_explicit(&director.usb_device_lost, memory_order_acquire) ? 1 : 0;
	return ret;
}
