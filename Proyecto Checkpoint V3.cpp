#include <iostream>   
#include <string>     
#include <vector>     
#include <iomanip>    
#include <fstream>    

using namespace std;


struct Alumno
{
    string nombre;
    string matricula;
    float calif[3]; 
    float promedio;
    int grupo;          
};


struct Maestro
{
    string nombre;
    string materia;
};


struct Grupo
{
    string nombre;
    string carrera;
};

// FUNCIONES PARA LEER DATOS DEL TECLADO

/*
 * leerNumero
 * Muestra un mensaje y lee un numero del teclado. Si el usuario escribe
 * algo que no es un numero (por ejemplo una letra), vuelve a pedirlo en
 * lugar de entrar en un ciclo infinito.
 * Parametro: mensaje - texto que se muestra al usuario.
 * Regresa: el numero capturado.
 */
float leerNumero(string mensaje)
{
    float valor;

    cout << mensaje;
    while (!(cin >> valor))         // Si la lectura falla (dato invalido)...
    {
        cin.clear();                // ...se limpia el estado de error de cin
        cin.ignore(1000, '\n');     // ...se descarta lo que escribio mal
        cout << "Dato invalido. " << mensaje;
    }
    cin.ignore(1000, '\n');         // Se quita el salto de linea que quedo pendiente
    return valor;
}

/*
 * leerTextoTeclado
 * Muestra un mensaje y lee una linea completa de texto (con espacios).
 * Parametro: mensaje - texto que se muestra al usuario.
 * Regresa: el texto capturado.
 */
string leerTextoTeclado(string mensaje)
{
    string texto;

    cout << mensaje;
    getline(cin, texto);
    return texto;
}


// FUNCIONES PARA LEER DATOS DE datos.txt

/*
 * saltarEtiqueta
 * Avanza en el archivo hasta pasar los dos puntos ":" y el espacio que
 * les sigue. Los titulos y las lineas en blanco (que no tienen ":")
 * tambien se saltan, porque ignore() avanza hasta encontrar el ":".
 * Parametro: archivo - archivo abierto para lectura.
 */
void saltarEtiqueta(ifstream& archivo)
{
    archivo.ignore(1000, ':');      // Descarta todo hasta el ":" (incluido)
    if (archivo.peek() == ' ')      // Si lo siguiente es un espacio...
    {
        archivo.get();              // ...tambien se descarta
    }
}

/*
 * leerTexto
 * Lee un dato de texto del archivo (lo que va despues de "Etiqueta: ").
 * Parametro: archivo - archivo abierto para lectura.
 * Regresa: el texto leido.
 */
string leerTexto(ifstream& archivo)
{
    string texto;

    saltarEtiqueta(archivo);
    getline(archivo, texto);        // Lee el resto de la linea
    return texto;
}

/*
 * leerDato
 * Lee un dato numerico del archivo (lo que va despues de "Etiqueta: ").
 * Se usa tanto para decimales (calificaciones) como para enteros
 * (cantidades y grupo); para los enteros se convierte con (int).
 * Parametro: archivo - archivo abierto para lectura.
 * Regresa: el numero leido (0 si no pudo leerlo).
 */
float leerDato(ifstream& archivo)
{
    float valor = 0;                // Se inicializa para no devolver basura

    saltarEtiqueta(archivo);
    archivo >> valor;
    archivo.ignore(1000, '\n');     // Descarta el resto de la linea
    return valor;
}

/*
 * guardarDatos
 * Escribe en datos.txt todos los grupos, maestros y alumnos que estan en
 * memoria. Si el archivo ya existe, se sobrescribe.
 * Parametros: alumnos, grupos, maestros - vectores con los datos a guardar.
 */
void guardarDatos(vector<Alumno>& alumnos,
    vector<Grupo>& grupos,
    vector<Maestro>& maestros)
{
    ofstream archivo("datos.txt");

    // Si no se pudo crear el archivo, se avisa y se termina la funcion
    if (!archivo)
    {
        cout << "Error al guardar.\n";
        return;
    }

    archivo << fixed << setprecision(2);    // Anadimos esto, para determinar la cantidad de decimales que queremos utilizar, en este caso utilizaremos 2 decimales

    // ----- Seccion de grupos -----
    archivo << "===== GRUPOS =====\n";
    archivo << "Total de grupos: " << grupos.size() << "\n\n";

    for (int i = 0; i < (int)grupos.size(); i++)
    {
        archivo << "Grupo " << i + 1 << "\n"
            << "Nombre: " << grupos[i].nombre << "\n"
            << "Carrera: " << grupos[i].carrera << "\n\n";
    }

    // ----- Seccion de maestros -----
    archivo << "===== MAESTROS =====\n";
    archivo << "Total de maestros: " << maestros.size() << "\n\n";

    for (int i = 0; i < (int)maestros.size(); i++)
    {
        archivo << "Maestro " << i + 1 << "\n"
            << "Nombre: " << maestros[i].nombre << "\n"
            << "Materia: " << maestros[i].materia << "\n\n";
    }

    // ----- Seccion de alumnos -----
    archivo << "===== ALUMNOS =====\n";
    archivo << "Total de alumnos: " << alumnos.size() << "\n\n";

    for (int i = 0; i < (int)alumnos.size(); i++)
    {
        Alumno& a = alumnos[i];             // Referencia para escribir mas corto
        string carrera = "(sin grupo)";

        // La carrera se obtiene del grupo del alumno (si el grupo es valido)
        if (a.grupo >= 0 && a.grupo < (int)grupos.size())
        {
            carrera = grupos[a.grupo].carrera;
        }

        archivo << "Alumno " << i + 1 << "\n"
            << "Nombre: " << a.nombre << "\n"
            << "Matricula: " << a.matricula << "\n"
            << "Calificacion 1: " << a.calif[0] << "\n"
            << "Calificacion 2: " << a.calif[1] << "\n"
            << "Calificacion 3: " << a.calif[2] << "\n"
            << "Promedio: " << a.promedio << "\n"
            << "Grupo: " << a.grupo + 1 << "\n"      // En el archivo el grupo empieza en 1
            << "Carrera: " << carrera << "\n\n";
    }

    cout << "\nDatos guardados correctamente.\n";
}

/*
 * cargarDatos
 * Lee datos.txt y llena los vectores con lo que encuentre, reemplazando
 * lo que hubiera en memoria. Si el archivo no existe, no hace nada.
 * Si el archivo esta danado, avisa y deja intactos los datos en memoria.
 * Parametros: alumnos, grupos, maestros - vectores que se van a llenar.
 */
void cargarDatos(vector<Alumno>& alumnos,
    vector<Grupo>& grupos,
    vector<Maestro>& maestros)
{
    ifstream archivo("datos.txt");

    // Si el archivo no existe, el programa inicia con todo vacio
    if (!archivo)
    {
        cout << "\nNo existe archivo. Iniciando vacio.\n";
        return;
    }

    // Se lee primero a vectores temporales: si el archivo esta danado
    // no se pierden los datos que ya estaban en memoria.
    vector<Grupo> tmpGrupos;
    vector<Maestro> tmpMaestros;
    vector<Alumno> tmpAlumnos;
    int cantidad;

    // Grupos
    cantidad = (int)leerDato(archivo);              // "Total de grupos"
    for (int i = 0; i < cantidad && archivo; i++)   // "&& archivo" corta si hubo error
    {
        Grupo g;
        g.nombre = leerTexto(archivo);
        g.carrera = leerTexto(archivo);
        tmpGrupos.push_back(g);
    }

    // Maestros
    cantidad = (int)leerDato(archivo);              // "Total de maestros"
    for (int i = 0; i < cantidad && archivo; i++)
    {
        Maestro m;
        m.nombre = leerTexto(archivo);
        m.materia = leerTexto(archivo);
        tmpMaestros.push_back(m);
    }

   
    // Alumnos
    cantidad = (int)leerDato(archivo);              // "Total de alumnos"
    for (int i = 0; i < cantidad && archivo; i++)
    {
        Alumno a;
        a.nombre = leerTexto(archivo);
        a.matricula = leerTexto(archivo);
        a.calif[0] = leerDato(archivo);
        a.calif[1] = leerDato(archivo);
        a.calif[2] = leerDato(archivo);
        leerDato(archivo);                          // "Promedio": solo informativo, se recalcula
        a.grupo = (int)leerDato(archivo) - 1;       // En el archivo empieza en 1; en memoria en 0
        leerTexto(archivo);                         // "Carrera": solo informativa
        a.promedio = (a.calif[0] + a.calif[1] + a.calif[2]) / 3;

        // Si el grupo del alumno no existe, el archivo se considera danado
        if (a.grupo < 0 || a.grupo >= (int)tmpGrupos.size())
        {
            a.grupo = -1;
            archivo.setstate(ios::failbit);
        }

        tmpAlumnos.push_back(a);
    }

    // Si hubo cualquier error de lectura, no se carga nada
    if (!archivo)
    {
        cout << "\nArchivo vacio o corrompido. No se cargaron datos.\n";
        return;
    }

    // Todo se leyo bien: ahora si se reemplazan los datos en memoria
    grupos = tmpGrupos;
    maestros = tmpMaestros;
    alumnos = tmpAlumnos;

    cout << "\nDatos cargados correctamente.\n";
}

// En este apartado se encuentra la parte del codigo encargada de generar el reporte en un archivo de texto llamado "reporte.txt

/*
 * generarReporte
 * Crea reporte.txt con los alumnos ordenados por promedio, de mayor a
 * menor, en forma de tabla (numero, nombre, matricula, grupo y promedio).
 * Recibe una COPIA del vector de alumnos para ordenarla sin cambiar el
 * orden original de los datos en memoria.
 * Parametros: alumnos - copia del vector de alumnos.
 *             grupos  - vector de grupos (para mostrar el nombre del grupo).
 */
void generarReporte(vector<Alumno> alumnos, vector<Grupo>& grupos)
{
    // Sin alumnos no hay nada que reportar
    if (alumnos.empty())
    {
        cout << "\nNo hay alumnos para el reporte.\n";
        return;
    }

    ofstream reporte("reporte.txt");

    if (!reporte)
    {
        cout << "Error al generar reporte.\n";
        return;
    }

    // Ordenamiento burbuja de mayor a menor promedio.
    // Solo se intercambia si el siguiente es ESTRICTAMENTE mayor, asi los
    // alumnos con el mismo promedio conservan su orden original.
    for (int i = 0; i < (int)alumnos.size() - 1; i++)
    {
        for (int j = 0; j < (int)alumnos.size() - 1 - i; j++)
        {
            if (alumnos[j + 1].promedio > alumnos[j].promedio)
            {
                Alumno aux = alumnos[j];
                alumnos[j] = alumnos[j + 1];
                alumnos[j + 1] = aux;
            }
        }
    }

    // Encabezado de la tabla. setw(n) reserva n espacios para cada columna
    // y left alinea el texto a la izquierda.
    reporte << "REPORTE DE ALUMNOS (ordenado por promedio, de mayor a menor)\n"
        << string(78, '=') << "\n"
        << left << setw(5) << "#" << setw(24) << "Nombre"
        << setw(12) << "Matricula" << setw(18) << "Grupo" << "Promedio\n"
        << string(78, '-') << "\n";

    // Una fila por alumno
    for (int i = 0; i < (int)alumnos.size(); i++)
    {
        string nombreGrupo = "(sin grupo)";

        if (alumnos[i].grupo >= 0 && alumnos[i].grupo < (int)grupos.size())
        {
            nombreGrupo = grupos[alumnos[i].grupo].nombre;
        }

        // substr(0, n) recorta el texto a n caracteres para que no se
        // desalineen las columnas cuando un dato es muy largo.
        reporte << left << setw(5) << i + 1
            << setw(24) << alumnos[i].nombre.substr(0, 23)
            << setw(12) << alumnos[i].matricula.substr(0, 11)
            << setw(18) << nombreGrupo.substr(0, 17)
            << fixed << setprecision(2) << alumnos[i].promedio << "\n";
    }

    cout << "\nReporte generado correctamente.\n";
}

// El programa principal inicia desde aqui

int main()
{
    int opcion;

    // Los datos del sistema viven en estos tres vectores mientras el programa esta abierto. Para conservarlos hay que usar "Guardar datos".
    
    vector<Alumno> alumnos;
    vector<Grupo> grupos;
    vector<Maestro> maestros;

    // Al iniciar se cargan automaticamente los datos guardados existentes
    cargarDatos(alumnos, grupos, maestros);

    // Ciclo del menu: se repite hasta que el usuario elija salir del sistema
    do
    {
        cout << "\n===== MENU =====\n"
            << "1. Capturar alumnos\n"
            << "2. Capturar grupos\n"
            << "3. Capturar maestros\n"
            << "4. Mostrar reportes\n"
            << "5. Guardar datos\n"
            << "6. Cargar datos\n"
            << "7. Generar reporte\n"
            << "8. Salir\n";

        opcion = (int)leerNumero("Opcion: ");

        switch (opcion)
        {
            //  Opcion 1: capturar los datos de la estructura alumnos 
        case 1:
        {
            // Un alumno necesita pertenecer a un grupo, asi que debe existir al menos uno
            if (grupos.empty())
            {
                cout << "\nPrimero capture un grupo.\n";
                break;
            }

            int cantidad;

            // Se pide la cantidad hasta que sea mayor a cero
            do
            {
                cantidad = (int)leerNumero("Cantidad de alumnos: ");
            } while (cantidad <= 0);

            for (int i = 0; i < cantidad; i++)
            {
                Alumno a;
                float suma = 0;

                cout << "\nAlumno " << i + 1 << endl;
                a.nombre = leerTextoTeclado("Nombre: ");
                a.matricula = leerTextoTeclado("Matricula: ");

                // Se capturan las 3 calificaciones y se van sumando
                for (int j = 0; j < 3; j++)
                {
                    a.calif[j] = leerNumero("Calificacion " + to_string(j + 1) + ": ");
                    suma += a.calif[j];
                }

                a.promedio = suma / 3;

                // Se muestran los grupos disponibles
                cout << "\nGrupos disponibles:\n";
                for (int k = 0; k < (int)grupos.size(); k++)
                {
                    cout << k + 1 << ". " << grupos[k].nombre
                        << " - " << grupos[k].carrera << endl;
                }

                // Solo se acepta un numero de grupo existente
                int seleccion;
                do
                {
                    seleccion = (int)leerNumero("Seleccione grupo: ");
                } while (seleccion < 1 || seleccion >(int)grupos.size());

                a.grupo = seleccion - 1;    // En memoria el indice empieza en 0
                alumnos.push_back(a);
            }
            break;
        }

        // ----- Opcion 2: capturar los datos de la estructura grupo -----
        case 2:
        {
            Grupo g;
            g.nombre = leerTextoTeclado("Nombre del grupo: ");
            g.carrera = leerTextoTeclado("Carrera: ");
            grupos.push_back(g);
            break;
        }

        // ----- Opcion 3: capturar los datos de la estructura maestro -----
        case 3:
        {
            Maestro m;
            m.nombre = leerTextoTeclado("Nombre del docente: ");
            m.materia = leerTextoTeclado("Materia: ");
            maestros.push_back(m);
            break;
        }

        // ----- Opcion 4: se encarga de imprimir los datos en la terminal
        case 4:
        {
            cout << "\n===== ALUMNOS =====\n";

            for (int i = 0; i < (int)alumnos.size(); i++)
            {
                cout << "\nNombre: " << alumnos[i].nombre << endl
                    << "Matricula: " << alumnos[i].matricula << endl
                    << "Promedio: " << fixed << setprecision(2)
                    << alumnos[i].promedio << endl;

                // La carrera se toma del grupo en caso que sea valido 
                if (alumnos[i].grupo >= 0 && alumnos[i].grupo < (int)grupos.size())
                {
                    cout << "Carrera: " << grupos[alumnos[i].grupo].carrera << endl;
                }
            }
            break;
        }

        // ----- Opcion 5: se encarga de guardar los datos en el archivo de texto datos.txt -----
        case 5:
            guardarDatos(alumnos, grupos, maestros);
            break;

            // ----- Opcion 6: cargar los datos desde el archivo de texto datos.txt -----
        case 6:
            cargarDatos(alumnos, grupos, maestros);
            break;

            // ----- Opcion 7: genera el archivo de texto reporte.txt -----
        case 7:
            generarReporte(alumnos, grupos);
            break;

            // ----- Opcion 8: salir (el ciclo termina porque la opcion es igual a 8) -----
        case 8:
            cout << "\nFin del programa.\n";
            break;

            // ----- Cualquier otra opcion -----
        default:
            cout << "\nOpcion invalida.\n";
        }

    } while (opcion != 8);

    return 0;
}