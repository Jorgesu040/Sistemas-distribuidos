# Librería de Utilidades de Red (Networking Utils)

¡Bienvenido/a a la Librería de Utilidades de Red! Esta librería ofrece herramientas para construir aplicaciones cliente-servidor en C++. Simplifica la creación de clientes y servidores, el envío/recepción de mensajes y la gestión de conexiones.

## Índice

- Descripción general
- Instalación
- Documentación
- Ejemplos
- Imágenes
- Diapositivas
- Cómo contribuir
- Licencia

## Descripción general

Networking Utils está pensada para ayudarte a crear aplicaciones de red robustas con menos fricción. Abstrae gran parte de la complejidad del manejo de sockets para que puedas centrarte en la lógica de tu aplicación.

## Instalación

Para usar la librería:

1. Clona el repositorio:

   ```bash
   git clone <repository-url>
   ```

2. Entra en el directorio del proyecto:

   ```bash
   cd networking-utils
   ```

3. Compila los ejemplos para ver cómo se usa la librería:

   ```bash
   g++ examples/*.cpp -o examples/output
   ```

## Documentación

Esta versión enlaza a los documentos en español (`.es.md`) dentro de `docs`:

- [Conceptos básicos](docs/basic-concepts.es.md)
- [Guía de cliente](docs/client-guide.es.md)
- [Guía de servidor](docs/server-guide.es.md)
- [Estructuras de datos](docs/data-structures.es.md)
- [Guía de mensajería](docs/messaging-guide.es.md)
- [Uso avanzado](docs/advanced-usage.es.md)

Si lo prefieres, también están disponibles las versiones originales en inglés (`.md`).

## Ejemplos

En `examples` encontrarás implementaciones de referencia:

- Cliente simple: `examples/simple-client.cpp`
- Servidor simple: `examples/simple-server.cpp`
- Servidor asíncrono: `examples/async-server.cpp`
- **Manejo de strings: `examples/string-example.cpp`** (muestra cómo enviar/recibir `std::string`)
- **Threading y mutex: `examples/threading-example.cpp`** (servidor multihilo con sincronización)

## Imágenes

- Diagrama cliente-servidor: `images/client-server-diagram.svg`
- Flujo de mensajes: `images/message-flow.svg`

## Diapositivas

- Presentación introductoria: `slides/intro-presentation.pptx`

## Cómo contribuir

¡Las contribuciones son bienvenidas! Haz un fork del repositorio y envía un pull request.

## Licencia

Proyecto con licencia MIT. Consulta el archivo LICENSE para más detalles.
