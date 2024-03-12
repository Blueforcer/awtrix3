# Librería Arduino Mean Filter
La librería Mean Filter implementa un filtro de media móvil. La librería almacena los N últimos elementos de la ventana y calcula la media. Se emplea un buffer circular para mantener la eficiencia alta.

Más información https://www.luisllamas.es/libreria-arduino-mean-filter/

## Instrucciones de uso
La clase Mean Filter emplea templates para permitir funcionar con distintos tipos (int, long, float,…). No obstante, hay que tener en cuenta que el filtro almacena N valores del tipo seleccionado, lo que puede causar desbordamiento si los valores filtrados y/o N son grandes. En este caso, hay que instanciar el filtro en un tipo de tamaño superior al de la señal filtrada (por ejemplo long para valores int).

### Constructor
El filtro de media móvil se instancia a través de su constructor que recibe el tamaño de la ventana como único parámetro.
```c++
MeanFilter<T> meanFilter(windowSize);
```

### Uso del filtro
```c++
// Añadir un nuevo valor al filtro y devolver el valor filtrado
meanFilter.AddValue(value);
 
//Obtiene el ultimo valor filtrado (el mismo que el devuelto al añadir el valor al filtro)
meanFilter.GetFiltered();
```


## Ejemplos
La librería Mean Filter incluye los siguientes ejemplos para ilustrar su uso.
* MeanFilterLong: Ejemplo de ordenación para variables long.
```c++
#include "MeanFilterLib.h"

// Simula la captura de un sensor o la lectura de un sensor
int values[] = { 7729, 7330, 10075, 10998, 11502, 11781, 12413, 12530, 14070, 13789, 18186, 14401, 16691, 16654, 17424, 21104, 17230, 20656, 21584, 21297, 19986, 20808, 19455, 24029, 21455, 21350, 19854, 23476, 19349, 16996, 20546, 17187, 15548, 9179, 8586, 7095, 9718, 5148, 4047, 3873, 4398, 2989, 3848, 2916, 1142, 2427, 250, 2995, 1918, 4297, 617, 2715, 1662, 1621, 960, 500, 2114, 2354, 2900, 4878, 8972, 9460, 11283, 16147, 16617, 16778, 18711, 22036, 28432, 29756, 24944, 27199, 27760, 30706, 31671, 32185, 32290, 30470, 32616, 32075, 32210, 28822, 30823, 29632, 29157, 31585, 24133, 23245, 22516, 18513, 18330, 15450, 12685, 11451, 11280, 9116, 7975, 8263, 8203, 4641, 5232, 5724, 4347, 4319, 3045, 1099, 2035, 2411, 1727, 852, 1134, 966, 2838, 6033, 2319, 3294, 3587, 9076, 5194, 6725, 6032, 6444, 10293, 9507, 10881, 11036, 12789, 12813, 14893, 16465, 16336, 16854, 19249, 23126, 21461, 18657, 20474, 24871, 20046, 22832, 21681, 21978, 23053, 20569, 24801, 19045, 20092, 19470, 18446, 18851, 18210, 15078, 16309, 15055, 14427, 15074, 10776, 14319, 14183, 7984, 8344, 7071, 9675, 5985, 3679, 2321, 6757, 3291, 5003, 1401, 1724, 1857, 2605, 803, 2742, 2971, 2306, 3722, 3332, 4427, 5762, 5383, 7692, 8436, 13660, 8018, 9303, 10626, 16171, 14163, 17161, 19214, 21171, 17274, 20616, 18281, 21171, 18220, 19315, 22558, 21393, 22431, 20186, 24619, 21997, 23938, 20029, 20694, 20648, 21173, 20377, 19147, 18578, 16839, 15735, 15907, 18059, 12111, 12178, 11201, 10577, 11160, 8485, 7065, 7852, 5865, 4856, 3955, 6803, 3444, 1616, 717, 3105, 704, 1473, 1948, 4534, 5800, 1757, 1038, 2435, 4677, 8155, 6870, 4611, 5372, 6304, 7868, 10336, 9091 };
size_t valuesLength = sizeof(values) / sizeof(values[0]);

int getMeasure()
{
	size_t static index = 0;
	index++;
	return values[index - 1];
}

// Instanciar filtro media movil con ventana tamaño 5
MeanFilter<long> meanFilter(5);

void setup()
{
	Serial.begin(115200);

	for (size_t index = 0; index < valuesLength; index++)
	{
		// Obtener medicion simulada
		int rawMeasure = getMeasure();

		// Calcular filtro
		int mean = meanFilter.AddValue(rawMeasure);

		// Mostrar resultados
		// Emplear Serial Plotter para visualización gráfica
		Serial.print(rawMeasure);
		Serial.print("\t");
		Serial.println(mean);
	}
}

void loop()
{  
}
```

* MeanFilterFloat: Ejemplo de ordenación para variables float.
```c++
#include "MeanFilterLib.h"

// Simula la captura de un sensor o la lectura de un sensor
float values[] = { 7729, 7330, 10075, 10998, 11502, 11781, 12413, 12530, 14070, 13789, 18186, 14401, 16691, 16654, 17424, 21104, 17230, 20656, 21584, 21297, 19986, 20808, 19455, 24029, 21455, 21350, 19854, 23476, 19349, 16996, 20546, 17187, 15548, 9179, 8586, 7095, 9718, 5148, 4047, 3873, 4398, 2989, 3848, 2916, 1142, 2427, 250, 2995, 1918, 4297, 617, 2715, 1662, 1621, 960, 500, 2114, 2354, 2900, 4878, 8972, 9460, 11283, 16147, 16617, 16778, 18711, 22036, 28432, 29756, 24944, 27199, 27760, 30706, 31671, 32185, 32290, 30470, 32616, 32075, 32210, 28822, 30823, 29632, 29157, 31585, 24133, 23245, 22516, 18513, 18330, 15450, 12685, 11451, 11280, 9116, 7975, 8263, 8203, 4641, 5232, 5724, 4347, 4319, 3045, 1099, 2035, 2411, 1727, 852, 1134, 966, 2838, 6033, 2319, 3294, 3587, 9076, 5194, 6725, 6032, 6444, 10293, 9507, 10881, 11036, 12789, 12813, 14893, 16465, 16336, 16854, 19249, 23126, 21461, 18657, 20474, 24871, 20046, 22832, 21681, 21978, 23053, 20569, 24801, 19045, 20092, 19470, 18446, 18851, 18210, 15078, 16309, 15055, 14427, 15074, 10776, 14319, 14183, 7984, 8344, 7071, 9675, 5985, 3679, 2321, 6757, 3291, 5003, 1401, 1724, 1857, 2605, 803, 2742, 2971, 2306, 3722, 3332, 4427, 5762, 5383, 7692, 8436, 13660, 8018, 9303, 10626, 16171, 14163, 17161, 19214, 21171, 17274, 20616, 18281, 21171, 18220, 19315, 22558, 21393, 22431, 20186, 24619, 21997, 23938, 20029, 20694, 20648, 21173, 20377, 19147, 18578, 16839, 15735, 15907, 18059, 12111, 12178, 11201, 10577, 11160, 8485, 7065, 7852, 5865, 4856, 3955, 6803, 3444, 1616, 717, 3105, 704, 1473, 1948, 4534, 5800, 1757, 1038, 2435, 4677, 8155, 6870, 4611, 5372, 6304, 7868, 10336, 9091 };
size_t valuesLength = sizeof(values) / sizeof(values[0]);

float getMeasure()
{
	size_t static index = 0;
	index++;
	return values[index - 1];
}

// Instanciar filtro media movil con ventana tamaño 5
MeanFilter<float> meanFilter(5);

void setup()
{
	Serial.begin(115200);

	for (size_t index = 0; index < valuesLength; index++)
	{
		// Obtener medicion simulada
		float rawMeasure = getMeasure();

		// Calcular filtro
		float mean = meanFilter.AddValue(rawMeasure);

		// Mostrar resultados
		// Emplear Serial Plotter para visualización gráfica
		Serial.print(rawMeasure);
		Serial.print("\t");
		Serial.println(mean);
	}
}

void loop()
{  
}
```
