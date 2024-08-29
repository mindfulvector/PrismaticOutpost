# 🌈 PrismaticOutpost

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
[![Qt](https://img.shields.io/badge/Qt-5.15%2B-green.svg)](https://www.qt.io)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Scheme](https://img.shields.io/badge/Scheme-R7RS-orange.svg)](https://small.r7rs.org/)

> *A spiritual successor to Frontier, reimagined for the modern era.*

PrismaticOutpost is a revolutionary, open-source development environment that empowers creators, developers, and business users alike.
It combines the flexibility of scripting, the power of databases, and the ease of use of modern applications into one seamless package.

## 🌟 Features

- **📊 Dock-based Tiling Interface**: Effortlessly manage multiple projects as separate Workspaces, with tiling window management for each workspace.
- **🚀 Scheme Scripting**: Harness the power of Scheme for automation and customization. Other scripting languages will be added as soon as possible.
- **🗄️ Hierarchical Data Storage**: SQLite-backed, accessible right inside your code as `dot.notation.paths.to.any.database.record`.
- **🌐 Web-Ready**: Run scripts in sandboxed environment directly within the interface, or deploy them online instantly as webassembly apps. Sensitive backend scripts can be triggered remotely with strong authentication and built-in SSL.
- **🔄 Cross-Platform**: Scripts are compiled to WebAssembly for unparalleled compatibility and execution speed.
- **🔗 Public Sharing**: Instantly share your work with ngrok integration without paying any hosting fees, or deploy it to our cloud infrastructure.
- **🐳 Docker Integration**: Seamless backend dependency management integrated via the use of Docker images.
- **📚 Documentation: Comprehensive documentation is available both in the program itself (and written using the same Markdown and scripting system as any other PrismaticOutpost document), as well as on Github in the [Wiki](https://github.com/mindfulvector/prismatic-outpost/wiki).
- **📝 Markdown Interface**: Not just for documents, we use Markdown files as a familiar, text-based control surface to manage static site creation and provide template files for scripts.

> *Please note, this project is in the very early stages and most of these features are not yet available. I plan to have them implemented very
quickly, however.*

## 🎯 Who It's For

- **Developers**: Streamline your workflow, from concept to deployment.
- **Business Users**: Create powerful, custom solutions without extensive coding knowledge required.
- **Data Analysts**: Manage, analyze, and visualize data with ease.
- **Educators**: Craft interactive learning experiences effortlessly.
- **Hobbyists**: Bring your ideas to life without the usual technical hurdles.

## 💡 Inspiration

PrismaticOutpost draws inspiration from Frontier, the groundbreaking userland environment. We aim to carry forward Frontier's
spirit of empowering users with a flexible, scriptable system, web-first and desktop-first system while embracing modern
technologies and development practices.

## 🚀 Getting Started

Prerequisites:

* IDE/Compiler: QtCreator 13.0.1
* Kit: Desktop Qt 6.7.1
* vcpkg (for third party Qt plugins)

```bash
git clone https://github.com/yourusername/prismatic-outpost.git
```

### Windows
To install `jq` and `conan` via `scoop.sh` (must already be installed following the Quickstart section at [scoop.sh](https://scoop.sh/#/)), run the `setup.bat` batch file:
```batch
setup.bat
```

### Linux/macOS
TODO

### Open the project and run it
* Open the directory as a project in QtCreator
* Ensure you are on Debug configuration
* Press F5 to run 


## Creating a PackageKit (either a debug or release build for distribution)
**Note:** This process uses CQtDeployer which you can install from that project's [releases page](https://github.com/QuasarApp/CQtDeployer/releases/tag/v1.6.2365).

To create a PackageKit release, in QtCreator:
1. Run **Build** > **Clean** (select the **Clean** command which is under __Build all Projects for All Configurations__)
2. Run **Build** > **Build all Projects for All Configurations**
3. In a terminal, at the root of the project, run:

```bash
/path/to/CQtDeployer -bin ./build/Desktop_Qt_6_7_1_MinGW_64_bit-Debug/debug/ -qmake /path/to/qmake -targetDir DistributionKit -verbose 3 -debug
```

then run
```bash
/path/to/CQtDeployer -bin ./build/Desktop_Qt_6_7_1_MinGW_64_bit-Release/release/ -qmake /path/to/qmake -targetDir DistributionKit -verbose 3 -debug
```

In both commands, adjust `/path/to/qmake` to the correct path and adjust `Desktop_Qt_6_7_1_MinGW_64_bit` to your exact point release of the Desktop Qt kit.

It is **highly** recommended that you build **both** debug and release PackageKits, and distribute **both** of them to aid in debugging issues that might be encountered.


## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

## 📄 License

PrismaticOutpost is open-source software licensed under the AGPL-3.0. The core engine is free to use, modify, and distribute.
You can also use it to create proprietary systems, provided that the databases and scripts are not packaged with the core
engine's source or binary distribution.

## 🌈 Join the Prismatic Revolution!

PrismaticOutpost is more than just a development environment—it's a *renewed* way of thinking about creating and managing digital
projects. Whether you're a seasoned developer, a business user looking for a powerful alternative to traditional tools like
Access or FileMaker, or simply someone with big ideas, PrismaticOutpost provides the canvas for your digital creations.

Ready to embark on a colorful journey of effortless creation? Star us on GitHub and join our community today!

---

*"In the right light, at the right time, everything is extraordinary." – [Aaron Rose](https://www.azquotes.com/author/38383-Aaron_Rose)*

Let your ideas shine with PrismaticOutpost! 🌈✨
