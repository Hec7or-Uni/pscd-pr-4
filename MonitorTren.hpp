# pragma once

# include <mutex>
# include <condition_variable>
# include <cstring>

using namespace std;

class MonitorTren {
private:
	// VARIABLES PERMANENTES
	int N_W;	// Tamaño del tren.
	int w;		// Personas dentro del vagón.
	bool puertasSalidaAbiertas;
	bool puertasEntradaAbiertas;
	// VARIABLES EXTRA
	bool limpiando;
	mutex mtx;	// Para la ejecución de procs en exclusión mutua.
	condition_variable trenVacio;	// Para esperar a que el tren esté vacı́o.
	// VARIABLES CONDICIÓN
	condition_variable conductor;	// Para esperar: [tren lleno, tren vacio, fin desinfección]
	condition_variable limpiar;		// Para espera a la desinfección del tren
	condition_variable puedesBajar;	// Para esperar a poder descender del tren

public:
	//------------------------- Constructor
    // Pre:  N_W >= 0
    // Post: El dato ha sido creado
    // Coms: Constructor con información para MonitorTren
	MonitorTren(const int N_W);
	//------------------------- Destructor
    // Pre:  True
    // Post: El dato ha sido destruído
    // Coms: Se invoca de manera automática al cerrarse el bloque donde ha sido declarado
	MonitorTren();
	//------------------------- Usuario
	// "i" es el identificador de usuario

	// Pre:  i > 0
    // Post: <await (w >= N_W) || !puertasEntradaAbiertas)
    //          trenVacio.wait(lck);
    //       >
	//		 <await (!puertasSalidaAbiertas)
    //          trenVacio.wait(lck);
    //       >
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	void monta(const int i);

	// Pre:  i > 0
    // Post: w-- y si w == 0 se avisa al conductor de que el tren está vacio
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	void desmonta(const int i);
	//------------------------- Conductor

	// Pre:  nV >= 0
    // Post: <await w < N_W 
    //          conductor.wait(lck);
    //      >
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void iniciaViaje(const int nV);

	// Pre:  nV >= 0
    // Post: Se abren las puertas de salida
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void avisaFinViaje(const int nV);

	// Pre:  nV >= 0
    // Post: <await w > 0 
    //          conductor.wait(lck)
    //       >
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void esperaHayanBajado(const int nV);

	// Pre:  nV >= 0
    // Post: Comienza desinfección del tren
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void avisaLimpieza(const int nV);

	// Pre:  nV >= 0
    // Post: <await limpiando
	//			conductor.wait(lck);
	//		 >
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void esperaFinLimpieza(const int nV);
	//------------------------- Servicio de limpieza
	// "nV" es el identificador del viaje

	// Pre:  nV >= 0
    // Post: <await !(limpiando) 
    //          limpiar.wait(lck);
    //      >
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void esperaAvisoInicio(const int nV);

	// Pre:  nV >= 0
    // Post: Avisa conductor desinfección terminada
    // Coms: En el caso de pasajeros con información de log, genera un evento en el fichero
	//		 "nV" es el identificador del viaje
	void avisaFinLimpieza(const int nV);
};
