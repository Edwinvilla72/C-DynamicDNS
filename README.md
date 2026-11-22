# Dynamic DNS (DDNS) in C
This is a small project that solves the problem of static DNS being unavailable. 
I was hosting a new personal site (with a laptop at my new apartment who offers Spectrum Wireless as the internet service provider). I learned quickly that they would allow me to forward ports, but not assign a static IP to my server. Apparently, only cool businesses can do that.
Rather than switching internet providers (which I wish I could do at this apartment), I decided to write a DynamicDNS solution to update my IP address on CloudFlare as soon as a new public IP is detected. But I couldn't do it in Python -- that would be lame. So I wrote it in C to learn how it all worked under the hood.

In all seriousness, this is a very basic project, but I had a lot of fun working on it and thought I would keep it here in case I needed it for another project, or maybe someone else could benefit. 
Of course, I wouldn't trust this for enterprise yet. If anyone finds any issues, please let me know!
