#include "plugin.hpp"
#include <configs/configs.hpp>

#include <fmt/format.h>
#include <fmt/args.h>

#define PLG_FMT_PAREN
#include <plg/formatter.hpp>

#include <plugify_export.h>

namespace ptf {
	/*plg::PluginResult TranslationsPlugin::OnPluginStart() {
	}

	plg::PluginResult TranslationsPlugin::OnPluginEnd() {
	}*/

	std::expected<void, std::string> TranslationsPlugin::LoadTranslation(const plg::vector<plg::string>& path) {
		auto config = configs::Config(path);
		if (auto error = config.GetError(); !error.empty()) {
			return std::unexpected("Reading error: {}", error);
		}


	}

	bool TranslationsPlugin::HasTranslation([[maybe_unused]] std::string_view key) {
		auto it1 = _translations.find(key);
		if (it1 == _translations.end())
			return false;

		return true;
	}

	bool TranslationsPlugin::HasTranslatedForLanguage([[maybe_unused]] std::string_view key, [[maybe_unused]] std::string_view lang){
		auto it1 = _translations.find(lang);
		if (it1 == _translations.end())
			return false;

		auto it2 = it1->second.find(key);
		if (it2 == it1->second.end())
			return false;

		return true;
	}

	void TranslationsPlugin::ReloadTransalations() {

	}

	std::string TranslationsPlugin::Translate(std::string_view lang, std::string_view key) const {
		auto it = _translations.find(key);
		if (it != _translations.end())
			return "";

		auto it2 = it->second.find(lang);
		if (it2 == it->second.end())
			return "";

		return it2->second;
	}
	
	std::string TranslationsPlugin::Translate(std::string_view lang, std::string_view key, std::span<const plg::any> args) const {
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

			return fmt::vformat(it2->second, store);
		}
		catch (const fmt::format_error& e) {
			return e.what();
		}
	}

	TranslationsPlugin plugin;
}// namespace ptf

PLUGIFY_PLUGIN(PLUGIFY_API, &ptf::plugin)
