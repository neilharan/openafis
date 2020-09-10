# OpenAFIS

**IN DEVELOPMENT: target completion early October 2020**.

A high performance one-to-many (1:N) fingerprint matching library for commodity hardware, written in modern platform independent C++.

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

Minutia Tensor Matrix: A New Strategy for Fingerprint Matching (https://doi.org/10.1371/journal.pone.0118910).

## Linear algebra

The algorithm, and indeed the fingerprint matching problem more generally, requires extensive use of relatively expensive matrix transformations.

https://medium.com/datathings/benchmarking-blas-libraries-b57fb1c6dc7

With typical matrix sizes being quite small (50-60 rows) the native C++ library Eigen is an obvious choice for this application (http://eigen.tuxfamily.org).

While GPU options like CUDA potentially enable massive parallelisation, the smaller matrix sizes used here do not warrant the extra cost of transferring data to and from the GPU memory.

A future study will explore whether a more complete CUDA implementation, with the majority of the algorithm implemented on the GPU rather than just BLAS-like primitives, is beneficial.

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

TODO

## Licensing

OpenAFIS is licensed under the BSD 2-Clause License. See [LICENSE][] for the full license text.

[LICENSE]: https://github.com/neilharan/openafis/blob/master/LICENSE
[EXTRACT]: https://github.com/neilharan/openafis/blob/master/data/extract.py
