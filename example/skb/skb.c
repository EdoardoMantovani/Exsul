/*
 * example created by GPT5.2, a simple kernel module which manipulates a skb,
 * this will be useful for testing the offset_generator subsystem, should compile under Linux 6.x.x
 */

// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>  /* struct ethhdr, ETH_HLEN */
#include <linux/ip.h>           /* struct iphdr */
#include <linux/printk.h>
#include <linux/byteorder/generic.h>

static unsigned int payload_len = 32;
module_param(payload_len, uint, 0644);
MODULE_PARM_DESC(payload_len, "Payload length to append to the skb");

static void dump_skb(const char *tag, const struct sk_buff *skb)
{
	pr_info("%s: skb=%p len=%u headroom=%u tailroom=%u data=%p head=%p end=%p\n",
		tag, skb, skb->len, skb_headroom(skb), skb_tailroom(skb),
		skb->data, skb->head, skb->end);

	print_hex_dump(KERN_INFO, tag, DUMP_PREFIX_OFFSET, 16, 1,
		       skb->data, skb->len, true);
}

static int __init skb_sandbox_init(void)
{
	struct sk_buff *skb = NULL, *skb_copy2 = NULL;
	u8 *p;
	struct ethhdr *eth;
	struct iphdr *iph;
	unsigned int i;

	pr_info("skb_sandbox: init (payload_len=%u)\n", payload_len);

	/* Allocate enough room for headers + payload + some headroom. */
	skb = alloc_skb(256 + payload_len, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	/*
	 * Reserve headroom so we can later "push" headers in front of payload
	 * without reallocating.
	 */
	skb_reserve(skb, 64);

	/* Append payload at the tail. */
	p = skb_put(skb, payload_len);
	for (i = 0; i < payload_len; i++)
		p[i] = (u8)(0xA0 + (i & 0x1F));  /* simple pattern */

	/*
	 * Now prepend ("push") fake IPv4 + Ethernet headers.
	 * This manipulates the skb buffer layout but does NOT send anything.
	 */
	iph = (struct iphdr *)skb_push(skb, sizeof(*iph));
	memset(iph, 0, sizeof(*iph));
	iph->version  = 4;
	iph->ihl      = 5; /* 20 bytes */
	iph->ttl      = 64;
	iph->protocol = IPPROTO_UDP; /* arbitrary for demonstration */
	iph->saddr    = cpu_to_be32(0x0a000001); /* 10.0.0.1 */
	iph->daddr    = cpu_to_be32(0x0a000002); /* 10.0.0.2 */
	iph->tot_len  = cpu_to_be16(sizeof(*iph) + payload_len);

	eth = (struct ethhdr *)skb_push(skb, ETH_HLEN);
	memset(eth, 0, ETH_HLEN);
	eth->h_proto = cpu_to_be16(ETH_P_IP);
	/* Fake MACs */
	eth->h_source[0] = 0x02;
	eth->h_dest[0]   = 0x02;
	eth->h_dest[5]   = 0x01;

	/* Tell skb where headers are (purely metadata). */
	skb_reset_mac_header(skb);
	skb_set_network_header(skb, ETH_HLEN);
	/* No real transport header here; still demonstrate setting an offset. */
	skb_set_transport_header(skb, ETH_HLEN + sizeof(*iph));

	dump_skb("original skb: ", skb);

	/*
	 * Make an independent copy (deep copy). Editing the copy will not
	 * affect the original.
	 */
	printk("skb copy ongoing..\n");
	skb_copy2 = skb_copy(skb, GFP_KERNEL);
	if (!skb_copy2) {
		kfree_skb(skb);
		return -ENOMEM;
	}
	printk("copied skb!\n");

	/* Mutate the copy: flip one byte in the payload area. */
	if (skb_copy2->len >= ETH_HLEN + sizeof(*iph) + 1) {
		u8 *payload = skb_copy2->data + ETH_HLEN + sizeof(*iph);
		payload[0] ^= 0xFF;
	}

	/* Trim the copy to remove the last 8 bytes (if possible). */
	if (skb_copy2->len > 8){
		skb_trim(skb_copy2, skb_copy2->len - 8);
	}

	dump_skb("copied/edited skb: ", skb_copy2);

	/*
	 * Demonstrate "pull": temporarily remove Ethernet header view
	 * (common when parsing). We'll put it back to keep the dump coherent.
	 */
	if (skb_pull(skb_copy2, ETH_HLEN)) {
		pr_info("after skb_pull(ETH_HLEN): len=%u (data points to IP header now)\n",
			skb_copy2->len);
		(void)skb_push(skb_copy2, ETH_HLEN);
	}

	kfree_skb(skb_copy2);
	kfree_skb(skb);

	pr_info("skb_sandbox: init done\n");
	return 0;
}

static void __exit skb_sandbox_exit(void)
{
	pr_info("skb_sandbox: exit\n");
}

module_init(skb_sandbox_init);
module_exit(skb_sandbox_exit);

MODULE_AUTHOR("example");
MODULE_DESCRIPTION("In-memory skb manipulation example (no NIC/network interaction)");
MODULE_LICENSE("GPL");
