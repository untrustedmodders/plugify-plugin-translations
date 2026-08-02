#pragma once

#include <span>
#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <expected>
#include <plg/format.hpp>
#include <plg/any.hpp>
#include <plg/plugin.hpp>

namespace ptf {
	class TranslationsPlugin final : public plg::Plugin {
	public:
		// IPluginEntry interface
		plg::PluginResult OnPluginStart() final;
		plg::PluginResult OnPluginEnd() final;

		void LoadTranslation(std::string_view path);
		bool HasTranslation(std::string_view key);
		bool HasTranslatedForLanguage(std::string_view key, std::string_view lang);
		void ReloadTransalations();

		std::string Translate(std::string_view lang, std::string_view key) const;
		std::string Translate(std::string_view lang, std::string_view key, std::span<const plg::any> args) const;

	private:
		std::unordered_map<std::string, std::unordered_map<std::string, std::string, plg::string_hash, std::equal_to<>>, plg::string_hash, std::equal_to<>> _translations;
		std::unordered_set<std::string, plg::string_hash, std::equal_to<>> _configs;
	};
}// namespace ptf
