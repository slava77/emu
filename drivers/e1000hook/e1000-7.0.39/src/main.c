
Under version 2.6 we never unpack all the sk buffs. This is
done at a higher level. The pages are mm from PCI. The 
following line kludges the driver to unpack these.


        // lsd  adapter->rx_ps_bsize0 = E1000_RXBUFFER_128;
        adapter->rx_ps_bsize0 = E1000_RXBUFFER_8192;
