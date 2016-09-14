// Pixel.t

template <typename T>
class Pixel {
	private:
		T red, blue, green;
	public:
		Pixel() {};
		Pixel(T r, T g, T b);
		inline T getRed()   const {return red; }
		inline T getBlue()  const {return blue; }
		inline T getGreen() const {return green; }

		void setRed(T r);
		void setGreen(T g);
		void setBlue(T b);
};

template <typename T>
Pixel<T>::Pixel(T r, T g, T b) {
	red   = r;
	green = g;
	blue  = b;
}


template <typename T>
void Pixel<T>::setRed(T r) {
	red = r;
}

template <typename T>
void Pixel<T>::setGreen(T g) {
	green = g;;
}

template <typename T>
void Pixel<T>::setBlue(T b) {
	blue = b;
}

