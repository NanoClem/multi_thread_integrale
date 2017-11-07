// Classe GrayImage

#ifndef GRAYIMAGE_HPP
#define GRAYIMAGE_HPP

#include <iostream>
#include <cstdint>
#include <fstream>

using namespace std;



class GrayImage {

	private :

		const uint16_t width, height;
		uint8_t * array;



	public :

		GrayImage() = delete;
		GrayImage(const GrayImage& src);
		~GrayImage();
		GrayImage(uint16_t w, uint16_t h);


		static GrayImage * readPGM(istream& is);
		void writePGM(ostream& os) const;


		//Getters et Setters
		inline const uint16_t& getWidth() const  {return width;}
		inline const uint16_t& getHeight() const {return height;}

		inline uint8_t& pixel(uint16_t x, uint16_t y) {return array[y*width+x];}								//Getter de modification
		inline const uint8_t& pixel(uint16_t x, uint16_t y) const {return array[y*width+x];}		//Getter de consultation


		//Operator
		GrayImage& operator =(const GrayImage& b) = delete;



		void clear(uint8_t color=00);
		void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);
		//void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);



		friend void skip_line(istream& is);
		friend void skip_comment(istream& is);

};

#endif
