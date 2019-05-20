/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/FreetypeFont.h>

#include FT_BITMAP_H

namespace mc {
	namespace gfx {
		namespace fty {
			namespace {
				Texture convertBitmapToTexture(const FT_Bitmap* bitmap) {
					TextureDesc desc = TextureDesc(bitmap->width, bitmap->rows);
					switch (bitmap->pixel_mode) {
					case FT_PIXEL_MODE_GRAY:
						desc.format = TextureDesc::Format::LUMINANCE;
						desc.internalFormat = TextureDesc::InternalFormat::RED;
						break;
					case FT_PIXEL_MODE_LCD:
						desc.width /= 3;
						desc.format = TextureDesc::Format::RGB;
						desc.internalFormat = TextureDesc::InternalFormat::RGB8;
						break;
					}
					desc.type = TextureDesc::Type::UNSIGNED_BYTE;
					desc.wrapS = TextureDesc::Wrap::CLAMP;
					desc.wrapT = TextureDesc::Wrap::CLAMP;
					desc.minFilter = TextureDesc::Filter::LINEAR;
					desc.magFilter = TextureDesc::Filter::LINEAR;

					Texture out = Texture(desc);

					PixelStorageHints hints{};
					hints.alignment = 1;
					out.setData(bitmap->buffer, 0, hints);

					return out;
				}

				MACE_NORETURN  void throwFreetypeError(const FT_Error& status, const std::string message, const unsigned int line, const char* file) {
					/*
					using a custom X-Macro instead of using FT_ERROR_H because
					certain error codes (like Cannot_Open_Resource) require custom
					error classes (for example, Invalid_File_Format needs to throw
					BadFileError instead of FontError)

					there is no way to do that using FreeType's built in error
					X-Macro as you can't do conditional macros
					*/
#define MACE__FREETYPE_ERROR_CASE_TYPE(err, msg, type) case FT_Err_##err: MACE__THROW_CUSTOM_LINE(type, std::to_string(status) + ": " + MACE_STRINGIFY(err) + ": " + msg, line, file); break;
#define MACE__FREETYPE_ERROR_CASE(err, msg) MACE__FREETYPE_ERROR_CASE_TYPE(err, msg, Font)
					switch (status) {
						MACE__FREETYPE_ERROR_CASE(Ok, "No error (invalid internal call to throwFreetypeError)")
							MACE__FREETYPE_ERROR_CASE_TYPE(Cannot_Open_Resource, "Unable to open font file", FileNotFound)
							MACE__FREETYPE_ERROR_CASE_TYPE(Unknown_File_Format, "Unknown file format", BadFile)
							MACE__FREETYPE_ERROR_CASE_TYPE(Invalid_File_Format, "Broken file", BadFile)
							MACE__FREETYPE_ERROR_CASE_TYPE(Invalid_Version, "Invalid FreeType version", InitializationFailed)
							MACE__FREETYPE_ERROR_CASE(Invalid_Argument, "Invalid argument")
							MACE__FREETYPE_ERROR_CASE(Unimplemented_Feature, "Unimplemented feature")
							MACE__FREETYPE_ERROR_CASE(Invalid_Glyph_Index, "Invalid glyph index")
							MACE__FREETYPE_ERROR_CASE(Invalid_Character_Code, "Invalid character code")
							MACE__FREETYPE_ERROR_CASE(Cannot_Render_Glyph, "Cannot render glyph")
							MACE__FREETYPE_ERROR_CASE(Invalid_Pixel_Size, "Invalid pixel size")
							MACE__FREETYPE_ERROR_CASE(Invalid_Handle, "Invalid object handle")
							MACE__FREETYPE_ERROR_CASE(Invalid_Library_Handle, "Invalid library handle")
							MACE__FREETYPE_ERROR_CASE(Invalid_Driver_Handle, "Invalid driver handle")
							MACE__FREETYPE_ERROR_CASE(Invalid_Face_Handle, "Invalid face handle")
							MACE__FREETYPE_ERROR_CASE(Invalid_Size_Handle, "Invalid size handle")
							MACE__FREETYPE_ERROR_CASE(Invalid_Slot_Handle, "Invalid slot handle")
							MACE__FREETYPE_ERROR_CASE_TYPE(Too_Many_Drivers, "Too many drivers", System)
							MACE__FREETYPE_ERROR_CASE_TYPE(Too_Many_Extensions, "Too many extensions", System)
							MACE__FREETYPE_ERROR_CASE_TYPE(Out_Of_Memory, "Out of memory", OutOfMemory)
					default:
						throw MACE__GET_ERROR_NAME(Font)(message + ": Unknown error code " + std::to_string(status), line, file);
					}
#undef MACE__FREETYPE_ERROR_CASE_TYPE
#undef MACE__FREETYPE_ERROR_CASE
				}

				inline void checkFreetypeError(const FT_Error& status, const std::string message, const unsigned int line, const char* file) {
					if (status != FT_Err_Ok) {
						throwFreetypeError(status, message, line, file);
					}
				}
			}//anonymous namespace

			FreetypeFont::FreetypeFont(const FontDesc& desc, FreetypeLibrary& lib) : library(lib.freetype) {
				switch (desc.loadType) {
				case FontLoadType::FILE:
					checkFreetypeError(FT_New_Face(library, desc.input.path, 0, &face), "Failed to create face", __LINE__, __FILE__);
					break;
				case FontLoadType::MEMORY:
					checkFreetypeError(FT_New_Memory_Face(library, desc.input.memory.data, static_cast<FT_Long>(desc.input.memory.size), 0, &face), "Failed to create face", __LINE__, __FILE__);
					break;
					default MACE_UNLIKELY:
					MACE__THROW(Font, "Unknown FontLoadType");
				}

				checkFreetypeError(FT_Select_Charmap(face, FT_ENCODING_UNICODE), "Failed to change charmap from font", __LINE__, __FILE__);
			}

			FreetypeFont::~FreetypeFont() {
				//checkFreetypeError(FT_Done_Face(face), "Failed to destroy face", __LINE__, __FILE__);
			}

			void FreetypeFont::fillGlyph(Glyph& out, const wchar_t character) const {
				checkFreetypeError(FT_Load_Char(face, character, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LCD), "Failed to load glyph", __LINE__, __FILE__);

				const WindowModule * window = gfx::getCurrentWindow();

				const FT_GlyphSlot glyph = face->glyph;
				const FT_Glyph_Metrics & gMetrics = glyph->metrics;
				const FT_Vector & advance = glyph->advance;
				out.metrics.width = window->convertPixelsToRelativeXCoordinates(gMetrics.width >> 6);
				out.metrics.height = window->convertPixelsToRelativeYCoordinates(gMetrics.height >> 6);
				out.metrics.bearingX = window->convertPixelsToRelativeXCoordinates(gMetrics.horiBearingY >> 6);
				out.metrics.bearingY = window->convertPixelsToRelativeYCoordinates(gMetrics.horiBearingY >> 6);
				out.metrics.advanceX = window->convertPixelsToRelativeXCoordinates(advance.x >> 6);
				out.metrics.advanceY = window->convertPixelsToRelativeYCoordinates(advance.y >> 6);

				// spaces and control characters have either 0 width or 0 height (or both!)
				if (out.metrics.width == 0.0f || out.metrics.height == 0.0f) {
					/*
					since these glyphs are used in a RGB blend,
					a #allblack texture will result in an invisible
					texture (which is what we want for 0 width glyphs)
					*/
					out.texture = Colors::BLACK;
				} else {
					// this monster of a line allows use to use RAII principles on targetBitmap with FT_Bitmap_Done
					/*
					using new FT_Bitmap here instead of making a stack variable because FT_Bitmap_Done is called
					in a destructor after the stack unwinds
					*/
					std::unique_ptr<FT_Bitmap, std::function<void(FT_Bitmap*)>> bitmapPtr = std::unique_ptr<FT_Bitmap, std::function<void(FT_Bitmap*)>>(new FT_Bitmap(), [&](FT_Bitmap * ptr) {
						// not calling checkFreetypeError here since this is a destructor and destructor's shouldn't throw
						// if the deallocation fails, then just ignore it
						FT_Bitmap_Done(library, ptr);

						delete ptr;
					});
					FT_Bitmap_New(bitmapPtr.get());

					// OpenGL requires uniform alignment so convert it to a proper alignment
					checkFreetypeError(FT_Bitmap_Convert(library, &glyph->bitmap, bitmapPtr.get(), 1), "Failed to convert bitmaps", __LINE__, __FILE__);

					bitmapPtr->pixel_mode = FT_PIXEL_MODE_LCD;

					out.texture = convertBitmapToTexture(bitmapPtr.get());
				}
			}

			void FreetypeFont::calculateMetricsForSize(const FontSize height) {
				const Vector<unsigned int, 2> dpi = getCurrentWindow()->getMonitor().getDPI();

				checkFreetypeError(FT_Set_Char_Size(face, 0, height << 6, dpi[0], dpi[1]), "Failed to change char size", __LINE__, __FILE__);
			}

			FontMetrics FreetypeFont::getFontMetrics() {
				const WindowModule* window = gfx::getCurrentWindow();

				FontMetrics out{};
				out.ascent = window->convertPixelsToRelativeYCoordinates(face->size->metrics.ascender >> 6);
				out.descent = window->convertPixelsToRelativeYCoordinates(face->size->metrics.descender >> 6);
				out.height = window->convertPixelsToRelativeYCoordinates(face->size->metrics.height >> 6);
				out.kerning = FT_HAS_KERNING(face);
				return out;
			}

			Vector<RelativeTranslation, 2> FreetypeFont::getKerning(const wchar_t prev, const wchar_t current) const {
				FT_Vector vec;

				checkFreetypeError(FT_Get_Kerning(face, prev, current, FT_KERNING_DEFAULT, &vec), "Failed to get kerning from face", __LINE__, __FILE__);

				return{static_cast<RelativeTranslation>(vec.x >> 6), static_cast<RelativeTranslation>(vec.y >> 6)};
			}

			void FreetypeLibrary::init() {
				checkFreetypeError(FT_Init_FreeType(&freetype), "Failed to initialize FreeType", __LINE__, __FILE__);
			}

			void FreetypeLibrary::destroy() {
				checkFreetypeError(FT_Done_FreeType(freetype), "Failed to destroy FreeType", __LINE__, __FILE__);
			}
		}//ogl33
	}//gfx
}//mc