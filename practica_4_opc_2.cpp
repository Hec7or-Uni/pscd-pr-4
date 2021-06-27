# include <iostream>
# include <chrono>
# include <time.h>
# include <mutex>
# include <thread>
# include <cstring>
# include "Semaphore_V4.hpp"
# include "MonitorTren.hpp"

using namespace std;

const int N = 48;	// Número de usuarios
const int N_W = 6;	// Capacidad del vagón
const int numViajes = N / N_W;	// Número de viajes

//------------------- LOGGER --------------------------
#ifdef LOGGING_MODE
    #include <Logger.hpp>
    Logger _logger("_log_.log", 1024);
    // Logger _logger("_log_2.log", cout, 1); 		// Mostrar eventos por stdout
    #define ADD_EVENT(e) {_logger.addMessage(e);}	// Generar evento
#else
    #define ADD_EVENT(e)
#endif

//----------------- DECLARACIÓN -----------------------

// Simula la subida y bajada de un usuario del tren
void usuario(MonitorTren& monTren, const int i);

// Gestiona el flujo de entrada y salida del tren y 
// la limpieza del mismo
void conductor(MonitorTren& monTren, const int numViajes);

// Desinfección del tren
void limpieza(MonitorTren& monTren, const int numViajes);

//----------------------- MAIN-------------------------
int main() {

 	MonitorTren mT(N_W); 
	thread t_usuario[N];
	thread t_conductor;
	thread t_limpieza;

	srand(time(NULL));
	//---------------- INICIO-PROCESOS --------------------
		ADD_EVENT("BEGIN_MAIN,0,0");
	t_conductor = thread(&conductor, ref(mT), numViajes);
	t_limpieza = thread(&limpieza, ref(mT), numViajes);
	for(int i = 0; i < N; i++) {
		t_usuario[i] = thread(&usuario, ref(mT), i);
	}
	
	t_conductor.join();
	t_limpieza.join();
	for(int i = 0; i < N; i++) {
		t_usuario[i].join();
	}
		ADD_EVENT("END_MAIN,0,0");
	//-----------------------------------------------------
	return 0;
}

//----------------- IMPLEMENTACIÓN --------------------
void usuario(MonitorTren& monTren, const int i) {
 	monTren.monta(i);
 	// simula cierto retraso a la hora de bajarse
	this_thread::sleep_for(chrono::milliseconds(5 + rand() % (10 + 1 - 5)));
 	monTren.desmonta(i);
}

void conductor(MonitorTren& monTren, const int numViajes) {
 	for (int i = 0; i < numViajes; i++) {
 		monTren.iniciaViaje(i);
		// simula tiempo de duración del viaje
		this_thread::sleep_for(chrono::milliseconds(10 + rand() % (15 + 1 - 10)));
		monTren.avisaFinViaje(i);
		monTren.esperaHayanBajado(i);
		monTren.avisaLimpieza(i);
		monTren.esperaFinLimpieza(i);
	}
}

void limpieza(MonitorTren& monTren, const int numViajes) {
 	for (int i = 0; i < numViajes; i++) {
		monTren.esperaAvisoInicio(i);
	 	// simula tiempo de duración de la limpieza
		this_thread::sleep_for(chrono::milliseconds(5 + rand() % (10 + 1 - 5)));
		monTren.avisaFinLimpieza(i);
 	}
}
//--------------------- FIN ---------------------------