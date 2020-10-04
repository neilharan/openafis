# OpenAFIS

A high-performance one-to-many (1:N) fingerprint matching library for commodity hardware, written in modern platform-independent C++.

Note: this library is focused on the matching problem. It does not currently extract minutiae from images.

The goal is to accurately identify one minutiae-set from 50K candidate sets within one second using modest laptop equipment.

## Progress

**Target is to be certification ready by mid-October 2020**.

  | TASK | COMPLETE | NOTES |
  | ---- | -------- | ----- |
  | Template loading | 100% | |
  | Local matching | 100% | |
  | Global matching | | |
  | CMake support | | |
  | Test suite | | EER, FMR100, FMR1000, ZeroFMR |
  | Benchmarks | | |
  | Optimizing | | float->int, vectorization (SSE/AVX), math functions |
  | Parallelizing | | |
  | Minutiae/pair rendering | 50% | SVG output |
  | Certification/evaluation | | FVC-onGoing, MINEX III (requires minutiae extraction function) |

## Supported operating systems

- Windows
- Linux
- Android
- iOS
- MacOS X
- And anywhere else you can find a C++17 toolchain

There is also a wrapper and qmake project file for incorporating with Qt projects.

## Test datasets

Tests and benchmarks are performed on freely available datasets from the Fingerprint Verification Competition hosted by the University of Bologna.

These data include several hundred reference fingerprints of varying quality:

- FVC2002 (http://bias.csr.unibo.it/fvc2002)
- FVC2004 (http://bias.csr.unibo.it/fvc2004)

The FVC archives are supplied in the tif raster format. A small python program [EXTRACT][] is provided to extract minutiae in ISO 19794-2:2005 format template files using SecuGens free SDK (https://secugen.com/products/sdk). Many fingerprint readers/SDKs can produce ISO format templates natively.

## Test readers/SDKs

- https://secugen.com/products/hamster-pro-20

## Algorithm

Improving Fingerprint Verification Using Minutiae Triplets (https://doi.org/10.3390/s120303418).

## Dependencies

Delaunay 2D Triangulation (https://github.com/delfrrr/delaunator-cpp) [MIT License]

## Supported minutiae template formats

- ISO/IEC 19794-2:2005 (https://www.iso.org/standard/38746.html)

## Example

```C++
TemplateISO19794_2_2005<unsigned short> t1(1);
assert(t1.load("./data/njh0.iso"));
std::cout << "template " << t1.id() << ": size " << t1.size() << " bytes, #fingerprints " << t1.lmts().size() << std::endl;

TemplateISO19794_2_2005<unsigned short> t2(2);
assert(t2.load("./data/njh1.iso"));
std::cout << "template " << t2.id() << ": size " << t2.size() << " bytes, #fingerprints " << t2.lmts().size()) << std::endl;

std::cout << "score " << Score<unsigned short>().compute(t1, t2);
```

## Benchmarking

### x86-64

  | METRIC | THREADS | OPTIMIZATION | PRODUCTION/RESEARCH | RESULT |
  | ------ | ------- | ------------ | ------------------- | ------ |
  | Load time¹ | | CPU | Production |
  | Memory usage | | CPU | Production | |
  | Memory usage | | Memory | Production | |
  | Memory usage | | CPU | Research | |
  | 1:N score time | 1 | CPU | Production | |
  | 1:N score time | 4 | CPU | Production | |
  | 1:N score time | | Memory | Production | |

### aarch64

  | METRIC | THREADS | OPTIMIZATION | PRODUCTION/RESEARCH | RESULT |
  | ------ | ------- | ------------ | ------------------- | ------ |
  | Load time¹ | | CPU | Production |
  | Memory usage | | CPU | Production | |
  | Memory usage | | Memory | Production | |
  | Memory usage | | CPU | Research | |
  | 1:N score time | 1 | CPU | Production | |
  | 1:N score time | 4 | CPU | Production | |
  | 1:N score time | | Memory | Production | |

¹ 19794-2:2005 templates pre-loaded in memory. The time taken to produce indexed in-memory templates is recorded (we're not measuring disk I/O here).

## Licensing

OpenAFIS is licensed under the BSD 2-Clause License. See [LICENSE][] for the full license text.

[LICENSE]: https://github.com/neilharan/openafis/blob/master/LICENSE
[EXTRACT]: https://github.com/neilharan/openafis/blob/master/data/extract.py
