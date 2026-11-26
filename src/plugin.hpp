#pragma once

#include <unordered_map>
#include <unordered_set>
#include <plg/format.hpp>
#include <plg/hash.hpp>
#include <plg/any.hpp>
#include <plg/plugin.hpp>

namespace ptf {
	class TranslationsPlugin final : public plg::Plugin {
	public:
		// IPluginEntry interface
		plg::PluginResult OnPluginStart() override;
		plg::PluginResult OnPluginEnd() override;

		plg::string LoadTranslation(const plg::vector<plg::string>& path);
		bool HasTranslation(const plg::string& key);
		bool HasTranslatedForLanguage(const plg::string& key, const plg::string& lang);
		void ReloadTransalations();
		void SetDefaultLanguage(const plg::string& lang);

		plg::string Translate(const plg::string& lang, const plg::string& key) const;
		plg::string Translate(const plg::string& lang, const plg::string& key, const plg::vector<plg::any>& args) const;

	private:
		const plg::string* Find(const plg::string& lang, const plg::string& key) const;

		std::unordered_map<plg::string, std::unordered_map<plg::string, plg::string, plg::string_hash, std::equal_to<>>, plg::string_hash, std::equal_to<>> _translations;
		std::unordered_set<plg::vector<plg::string>> _configs;
		plg::string _defaultLanguage;
	};
}// namespace ptf
