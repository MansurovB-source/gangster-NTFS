#include "device.h"

int init_cache(blkid_cache *cache){
	int status = blkid_get_cache(cache, NULL);
	if(status < 0) {
		puts("ERROR: Can't get the cache");
		return -1;
	}
	puts("Successful cache read");
	return 0; 
}

int probe_devices(blkid_cache *cache){
	int status = blkid_probe_all(*cache);
	if(status < 0){
		puts("ERROR: Can't probe all block devices");
		return -1;
	}
	puts("Successfully probed all block devices");
	return 0;
}

int iterate_devices(blkid_cache *cache){
	blkid_dev device;
	blkid_dev_iterate iterator = blkid_dev_iterate_begin(*cache);

	puts("Partitions:");
	while(blkid_dev_next(iterator, &device) == 0){
		const char *device_name = blkid_dev_devname(device);
		printf("\t%s\t", device_name);

		blkid_probe probe = blkid_new_probe_from_filename(device_name);
		if(probe == NULL) {
			puts("fuck\n");
		} else {
			blkid_loff_t size = blkid_probe_get_size(probe);
			size_print(size);
			blkid_do_probe(probe);

			get_tag(&probe, "UUID");
			get_tag(&probe, "TYPE");
		}
	}
	blkid_dev_iterate_end(iterator);
}

void size_print(long long size) {
	if(size >= _TiB_){
		printf("%lld TiB\n", (long long) (size / _TiB_));
	} else if(size >= _GiB_){
		printf("%lld Gib\n", (long long) (size / _GiB_));
	} else if(size >= _MiB_){
		printf("%lld Mib\n", (long long) (size / _MiB_));
	} else if(size >= _KiB_){
		printf("%lld Kib\n", (long long) (size / _KiB_));
	} else {
		printf("%lld B\n", (long long) (size / _KiB_));
	}
}

void get_tag(blkid_probe *probe, char *tag_name){
	const char *value;
	if(blkid_probe_has_value(*probe, tag_name) == 1){
		blkid_probe_lookup_value(*probe, tag_name, &value, NULL);
		printf("%s = %s\t", tag_name, value);
	}
}