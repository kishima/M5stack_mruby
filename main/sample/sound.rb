R"( #"

include M5

Lcd::text(100,120,"Play music!",Color.new(255,255,255))

Sound::load_mml 0,"T72L4O4V100 A32<C32E32G+32 A<ED8C8>B8A8 G8.G16A8B8A4.R8"
Sound::load_mml 1,"T72L16O4V60 R8 EC>A<CEAE>A<AFC>A<FD>B<D E>AFA<E>A<EGFC>A<CFC>A<C"
Sound::play_all(true)



)"