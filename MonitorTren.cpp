# include "MonitorTren.hpp"

//----------------DECLARACIÓN-LOGGER-------------------
#ifdef LOGGING_MODE
    #include <Logger.hpp>
    extern Logger _logger;
    #define ADD_EVENT(e) {_logger.addMessage(e);} // generar evento
#else
    #define ADD_EVENT(e)
#endif
//-----------------------------------------------------

MonitorTren::MonitorTren(const int N_W) {
	this->N_W = N_W;
	w = 0;
	puertasSalidaAbiertas = false;
	puertasEntradaAbiertas = true;
	limpiando = false;
}

MonitorTren::MonitorTren() {
}

// "i" es el identificador de usuario
void MonitorTren::monta(const int i) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_monta," + to_string(i) + "," + to_string(w));

	while ((w >= N_W) || !(puertasEntradaAbiertas)) {	// Mientras tren = lleno y puertasEntrada = abiertas => me bloqueo: tren al máximo.
		trenVacio.wait(lck);
	}
	w++;	// Usurio sube al tren.
	if (w == N_W) conductor.notify_all();	// Si tren = lleno: Aviso al conductor.
	while (!puertasSalidaAbiertas) {		// Si puertasSalida = cerradas => me bloqueo: Viaje en curso.
		puedesBajar.wait(lck);
	}
		ADD_EVENT("END_monta," + to_string(i) + "," + to_string(w));
}

void MonitorTren::desmonta(const int i) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_desmonta," + to_string(i) + "," + to_string(w));
	w--;			// Usuario baja del tren.
	if (w == 0) conductor.notify_all();	// Si tren = vacio: Aviso al conductor.
		ADD_EVENT("END_desmonta," + to_string(i) + "," + to_string(w));
}

// "nV" es el identificador del viaje
void MonitorTren::iniciaViaje(const int nV) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_iniciaViaje," + to_string(nV) + "," + to_string(w));
	puertasEntradaAbiertas = true;
	puertasSalidaAbiertas = false;
	while (w < N_W) {	// Espera tren lleno
		conductor.wait(lck);
	}
	puertasEntradaAbiertas = false;	// Tren lleno: cierra puertas para que no suba nadie más.
		ADD_EVENT("END_iniciaViaje," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::avisaFinViaje(const int nV) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_avisaFinViaje," + to_string(nV) + "," + to_string(w));
	puertasSalidaAbiertas = true;	// Abre puertas para que bajen los usuarios
		ADD_EVENT("END_avisaFinViaje," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::esperaHayanBajado(const int nV) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_esperaHayanBajado," + to_string(nV) + "," + to_string(w));
	puedesBajar.notify_all();	// todos los usuarios ya se pueden bajar
	if ( w > 0) {
		conductor.wait(lck);
	}
		ADD_EVENT("END_esperaHayanBajado," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::avisaLimpieza(const int nV) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_avisaLimpieza," + to_string(nV) + "," + to_string(w));
	limpiando = true;	// El tren se va a limpiar.
	limpiar.notify_all();
		ADD_EVENT("END_avisaLimpieza," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::esperaFinLimpieza(const int nV) {
	unique_lock<mutex> lck(mtx);
		ADD_EVENT("BEGIN_esperaFinLimpieza," + to_string(nV) + "," + to_string(w));
	if (limpiando) {	// Espera fin desinfección
		conductor.wait(lck);
	}
	puertasSalidaAbiertas = false;	// Tren vacio: conductor cierra puertas de salida.
	puertasEntradaAbiertas = true;	// Tren limpio, se abren las puertas de entrada.
	trenVacio.notify_all();
		ADD_EVENT("END_esperaFinLimpieza," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::esperaAvisoInicio(const int nV) {
	unique_lock<mutex> lck(mtx); // coge el mutex para exlusión  mutua
		ADD_EVENT("BEGIN_esperaAvisoInicio," + to_string(nV) + "," + to_string(w));
	if (!limpiando) {	// Espera a poder limpiar
		limpiar.wait(lck);
	}
		ADD_EVENT("END_esperaAvisoInicio," + to_string(nV) + "," + to_string(w));
}

void MonitorTren::avisaFinLimpieza(const int nV) {
	unique_lock<mutex> lck(mtx); // coge el mutex para exlusión  mutua
		ADD_EVENT("BEGIN_avisaFinLimpieza," + to_string(nV) + "," + to_string(w));
	limpiando = false;		// Se ha terminado la limpieza y no queda nada que limpiar.
	conductor.notify_all();	// Limpieza acabada: Aviso al conductor.
		ADD_EVENT("END_avisaFinLimpieza," + to_string(nV) + "," + to_string(w));
}