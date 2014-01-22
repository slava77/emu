//#define PAGE_SIZE 4096
#define BIGPHYS_PAGES_4 5000
#define RING_PAGES_4 1000
#define RING_ENTRY_LENGTH 8
#define MAXPACKET_4 9100
//#define MAXEVENT_4 30100
#define MAXEVENT_4 180000
#define TAILMEM 100
#define TAILPOS  80

#ifdef _DDU_SPY_
#define SKB_EXTRA 12 // skb->len is not the actual lenght. skb->len+SKB_EXTRA is.
#define SKB_OFFSET -14 // Data doesn't start at skb->data, but at skb->data+SKB_OFFSET.
#endif

#ifdef _DMB_SPY_
#define SKB_EXTRA -2 // With standard-compliant Ethernet header. Discard the appended 2 bytes of the packet counter.
#define SKB_OFFSET 0 // With standard-compliant Ethernet header (with 6 bytes MAC dest + 6 bytes MAC source + 2 bytes EtherType), no offset is needed.
#endif
