#import "LRUDiskCache.h"
#import "DFDiskCache.h"

@interface LRUDiskCache()
@property (nonatomic, retain) DFDiskCache *cache;
@end

@implementation LRUDiskCache 

- (instancetype)initWithName:(NSString *)name {
	if (self = [super init]) {
		self.cache = [[DFDiskCache alloc] initWithName:name];
	}
	return self;
}

- (nullable NSData *)dataForKey:(NSString *)key {
	return [self.cache dataForKey:key];
}

- (void)setData:(NSData *)data forKey:(NSString *)key {
	[self.cache setData:data forKey:key];
}

- (void)removeDataForKey:(NSString *)key {
	[self.cache removeDataForKey:key];
}

- (void)removeAllData {
	[self.cache removeAllData];
}

- (BOOL)containsDataForKey:(NSString *)key {
	return [self.cache containsDataForKey:key];
}

@end
