cmake --build out --config Release
echo i1.jpeg
./out/artifacts/cbrunsli i1.jpeg i1.brn --bench
echo i2.jpeg
./out/artifacts/cbrunsli i2.jpeg i2.brn --bench
echo i3.jpeg
./out/artifacts/cbrunsli i3.jpeg i3.brn --bench

echo restore i1.brn
./out/artifacts/dbrunsli i1.brn ri1.jpeg --bench
echo restore i2.brn
./out/artifacts/dbrunsli i2.brn ri2.jpeg --bench
echo restore i3.brn
./out/artifacts/dbrunsli i3.brn ri3.jpeg --bench