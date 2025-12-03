/**
 * @file storage.cpp
 * @author Jordi Gauchía (jgauchia@jgauchia.com)
 * @brief  Storage definition and functions
 * @version 0.2.3
 * @date 2025-11
 */

#include "storage.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include <cmath>
#include <sstream>
#include <iomanip>

#define SD_OCR_SDHC_CAP (1 << 30) /**< SD card SDHC capacity flag */

extern const uint8_t SD_CS;   /**< Chip Select pin for SD card */
extern const uint8_t SD_MISO; /**< MISO pin for SD card */
extern const uint8_t SD_MOSI; /**< MOSI pin for SD card */
extern const uint8_t SD_CLK;  /**< Clock pin for SD card */

static const char *TAG = "Storage";

/**
 * @brief Formats a byte size as a human-readable string with appropriate units.
 *
 * @details Converts a size in bytes to a string with units (B, KB, MB, GB, TB), using
 * 			two decimal places of precision.
 *
 * @param size Size in bytes
 * @return std::string Human-readable string (e.g. "12.34 MB")
 */
namespace 
{
	std::string formatSize(uint64_t size)
	{
		static const char *suffixes[] = {"B","KB","MB","GB","TB"};
		int order = 0;
		double formatted_size = static_cast<double>(size);
		while (formatted_size >= 1024 && order < sizeof(suffixes) / sizeof(suffixes[0]) - 1)
		{
			order++;
			formatted_size /= 1024;
		}
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << formatted_size << " " << suffixes[order];
		return oss.str();
	}
}

/**
 * @brief Storage Class constructor
 */
Storage::Storage() : isSdLoaded(false), card(nullptr) {}

/**
 * @brief Initialize the SD card
 * @return esp_err_t result code
 */
esp_err_t Storage::initSD()
{
		pinMode(GPIO_NUM_15, OUTPUT);
		digitalWrite(GPIO_NUM_15, LOW);

		SPI.begin(GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_11);
		
		if (!SD.begin(GPIO_NUM_15, SPI, 20000000, "/sdcard")) 
		{
			ESP_LOGE(TAG,"SD Card Mount Failed");
			isSdLoaded = false;
			return ESP_FAIL;
		} 
		else 
		{
			ESP_LOGI(TAG,"SD Card Mounted");
			isSdLoaded = true;
			return ESP_OK;
		}

}

/**
 * @brief Initialize SPIFFS filesystem
 * @return esp_err_t result code
 */
esp_err_t Storage::initSPIFFS()
{
	ESP_LOGI(TAG, "Initializing SPIFFS");

	esp_vfs_spiffs_conf_t conf =
		{
			.base_path = "/spiffs",
			.partition_label = NULL,
			.max_files = 5,
			.format_if_mount_failed = false};

	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK)
	{
		if (ret == ESP_FAIL)
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		else if (ret == ESP_ERR_NOT_FOUND)
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		else
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		return ESP_FAIL;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total, &used);
	if (ret != ESP_OK)
		ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
	else
		ESP_LOGI(TAG, "Partition size: total: %d used: %d", total, used);

	return ESP_OK;
}

/**
 * @brief Get SD card information
 * @return SDCardInfo structure containing SD card details
 */
SDCardInfo Storage::getSDCardInfo()
{
	SDCardInfo info;
	
	uint8_t cardType = SD.cardType();
	if (cardType == CARD_MMC)
		info.card_type = "MMC";
	else if (cardType == CARD_SD)
		info.card_type = "SDSC";
	else if (cardType == CARD_SDHC)
		info.card_type = "SDHC";
	else
		info.card_type = "UNKNOWN";

	info.total_space = formatSize(SD.cardSize());
	info.free_space = formatSize((SD.totalBytes() - SD.usedBytes()));
	info.used_space = formatSize(SD.usedBytes());  

	return info;
}


/**
 * @brief Check if SD card is loaded
 * @return true if SD card is loaded, false otherwise
 */
bool Storage::getSdLoaded() const
{
  	return isSdLoaded;
}

/**
 * @brief Open a file at the specified path
 * @param path File path
 * @param mode File open mode
 * @return FILE* pointer to opened file, or nullptr on failure
 */
FILE *Storage::open(const char *path, const char *mode)
{
  	return fopen(path, mode);
}

/**
 * @brief Close an open file
 * @param file FILE* pointer to file
 * @return Result of fclose (0 on success, EOF on error)
 */
int Storage::close(FILE *file)
{
 	return fclose(file);
}

/**
 * @brief Get the size of a file on the SD card
 * 
 * @param path Path to the file
 * @return size_t Size of the file in bytes
 */
size_t Storage::size(const char *path)
{
	struct stat st;
	if (stat(path, &st) == 0)
		return st.st_size; 
	return 0;
}

/**
 * @brief Read from a file into a uint8_t buffer
 * @param file FILE* pointer
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read
 */
size_t Storage::read(FILE *file, uint8_t *buffer, size_t size)
{
	if (!file)
		return 0;
	return fread(buffer, 1, size, file);
}

/**
 * @brief Read from a file into a char buffer
 * @param file FILE* pointer
 * @param buffer Buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read
 */
size_t Storage::read(FILE *file, char *buffer, size_t size)
{
	if (!file)
		return 0;
	return fread(buffer, 1, size, file);
}

/**
 * @brief Write from a uint8_t buffer to a file
 * @param file FILE* pointer
 * @param buffer Buffer to write from
 * @param size Number of bytes to write
 * @return Number of bytes written
 */
size_t Storage::write(FILE *file, const uint8_t *buffer, size_t size)
{
	if (!file)
		return 0;
	return fwrite(buffer, 1, size, file);
}

/**
 * @brief Write from a char buffer to a file
 * @param file FILE* pointer
 * @param buffer Buffer to write from
 * @param size Number of bytes to write
 * @return Number of bytes written
 */
size_t Storage::write(FILE *file, const char *buffer, size_t size)
{
	if (!file)
		return 0;
	return fwrite(buffer, 1, size, file);
}

/**
 * @brief Check if a file or directory exists
 * @param path File or directory path
 * @return true if it exists, false otherwise
 */
bool Storage::exists(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0;
}

/**
 * @brief Create a directory
 * @param path Directory path
 * @return true on success, false on failure
 */
bool Storage::mkdir(const char *path)
{
	return ::mkdir(path, 0777) == 0;
}

/**
 * @brief Remove a file
 * @param path File path
 * @return true on success, false on failure
 */
bool Storage::remove(const char *path)
{
  	return ::remove(path) == 0;
}

/**
 * @brief Remove a directory
 * @param path Directory path
 * @return true on success, false on failure
 */
bool Storage::rmdir(const char *path)
{
	return ::rmdir(path) == 0;
}

/**
 * @brief Set the file position indicator
 * @param file FILE* pointer
 * @param offset Offset in bytes
 * @param whence Position from where offset is added (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return Result of fseek (0 on success, nonzero on error)
 */
int Storage::seek(FILE *file, long offset, int whence)
{
	if (!file)
		return -1;
	return fseek(file, offset, whence);
}

/**
 * @brief Print a string to a file
 * @param file FILE* pointer
 * @param str String to print
 * @return Number of characters written, or a negative value if an error occurs
 */
int Storage::print(FILE *file, const char *str)
{
	if (!file)
		return -1;
	return fprintf(file, "%s", str);
}

/**
 * @brief Print a string with a newline to a file
 * @param file FILE* pointer
 * @param str String to print
 * @return Number of characters written, or a negative value if an error occurs
 */
int Storage::println(FILE *file, const char *str)
{
	if (!file)
		return -1;
	return fprintf(file, "%s\n", str);
}

/**
 * @brief Get number of available bytes left in the file
 * @param file FILE* pointer
 * @return Number of available bytes
 */
size_t Storage::fileAvailable(FILE *file)
{
	if (!file)
		return 0;
	long current_pos = ftell(file);
	fseek(file, 0, SEEK_END);
	long end_pos = ftell(file);
	fseek(file, current_pos, SEEK_SET);
	return end_pos - current_pos;
}

