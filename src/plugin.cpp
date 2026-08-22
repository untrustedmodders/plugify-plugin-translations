#include "plugin.hpp"
#include <configs/configs.hpp>

#include <fmt/format.h>
#include <fmt/args.h>

#define PLG_FMT_PAREN
#include <plg/formatter.hpp>

#include <plugin_export.h>

namespace ptf {
	plg::PluginResult TranslationsPlugin::OnPluginStart() {
		return {};
	}

	plg::PluginResult TranslationsPlugin::OnPluginEnd() {
		_translations.clear();
		_configs.clear();
		return {};
	}

	plg::string TranslationsPlugin::LoadTranslation(const plg::vector<plg::string>& paths) {
		if (_configs.contains(paths)) {
			return "";
		}

		configs::Config config(paths);
		if (!config) {
			return configs::Config::GetError();
		}

		if (config.IsObject() && config.JumpFirst()) {
			do {
				plg::string key = config.GetName();
				if (config.IsObject() && config.JumpFirst()) {
					auto& langs = _translations[key];
					do {
						langs[config.GetName()] = config.GetString();
					} while (config.JumpNext());
					config.JumpBack();
				}
			} while (config.JumpNext());
		}

		_configs.insert(paths);
		return "";
	}

	bool TranslationsPlugin::HasTranslation(const plg::string& key) {
		return _translations.contains(key);
	}

	bool TranslationsPlugin::HasTranslatedForLanguage(const plg::string& key, const plg::string& lang) {
		auto it = _translations.find(key);
		if (it == _translations.end())
			return false;

		return it->second.contains(lang);
	}

	void TranslationsPlugin::ReloadTransalations() {
		auto configs = std::move(_configs);
		_translations.clear();
		_configs.clear();

		for (const auto& paths : configs) {
			LoadTranslation(paths);
		}
	}

	plg::string TranslationsPlugin::Translate(const plg::string& lang, const plg::string& key) const {
		auto it = _translations.find(key);
		if (it == _translations.end())
			return "";

		auto it2 = it->second.find(lang);
		if (it2 == it->second.end())
			return "";

		return it2->second;
	}

	plg::string TranslationsPlugin::Translate(const plg::string& lang, const plg::string& key, const plg::vector<plg::any>& args) const {
		auto it1 = _translations.find(key);
		if (it1 == _translations.end())
			return "";

		auto it2 = it1->second.find(lang);
		if (it2 == it1->second.end())
			return "";

		try
		{
			fmt::dynamic_format_arg_store<fmt::format_context> store;
			store.reserve(args.size(), args.size());
			for (const auto& arg : args)  {
				store.push_back(arg);
			}

			auto out = fmt::memory_buffer();
			fmt::vformat_to(std::back_inserter(out), std::string_view(it2->second), store);
			return { out.data(), out.size() };
		}
		catch (const fmt::format_error& e) {
			return e.what();
		}
	}

	TranslationsPlugin plugin;
}// namespace ptf

PLUGIFY_WARN_PUSH()
PLUGIFY_LINKAGE()

extern "C" {
	PLUGIN_API plg::string LoadTranslation(const plg::vector<plg::string>& paths) {
		return ptf::plugin.LoadTranslation(paths);
	}

	PLUGIN_API void ReloadTransalations() {
		ptf::plugin.ReloadTransalations();
	}

	PLUGIN_API bool HasTranslation(const plg::string& key) {
		return ptf::plugin.HasTranslation(key);
	}

	PLUGIN_API bool HasTranslatedForLanguage(const plg::string& key, const plg::string& lang) {
		return ptf::plugin.HasTranslatedForLanguage(key, lang);
	}

	PLUGIN_API plg::string Translate(const plg::string& lang, const plg::string& key) {
		return ptf::plugin.Translate(lang, key);
	}

	PLUGIN_API plg::string TranslateFormat(const plg::string& lang, const plg::string& key, const plg::vector<plg::any>& args) {
		return ptf::plugin.Translate(lang, key, args);
	}
}
PLUGIFY_WARN_POP()

PLUGIFY_PLUGIN(PLUGIN_API, &ptf::plugin)
