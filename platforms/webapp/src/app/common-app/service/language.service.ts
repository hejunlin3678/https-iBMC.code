import { Injectable } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { CommonData } from 'src/app/common-app/models/common.model';

@Injectable()

export class LanguageService {
	constructor(
		private translate: TranslateService
	) { }

	/**
	 * 定义语言选项
	 * 默认只支持中英文，法日俄文需要后端支持。
	 */
	public langOptions: any[] = CommonData.LANG_OPTIONS;
	// 当前得语言关键字
	public locale: string = localStorage.getItem('locale') || '';

	// 当前选中得语言对象，初始化时与当前语言关键字对应
	public activeLanguage;

	// 获取应该使用的语言
	public getTargetLanguage(): string {
		let lang = '';
		/**
		 * 目标语言获取逻辑
		 * 1.从接口查询出支持的语种（登录页）。
		 * 2.从本地是否存有locale字段。若存在，则判断是否在支持列表内（支持则直接使用，不支持则下一步）。
		 * 3.查询浏览器默认语言，若在支持范围内，则使用浏览器当前语言。否则使用英文。
		 */
		let lastLang = localStorage.getItem('locale');
		lastLang = lastLang || '';
		// 这里获取的是  zh,en,fr,ja这样的通用语言，fr-FR这种是代表使用fr语言的具体国家
		const browserLang = this.translate.getBrowserLang();
		// 获取后端支持的语言列表
		const supportLang = this.langOptions.filter((itemLang) => {
			return !itemLang.hidden;
		});

		// 查询上次使用的语言是否在列表中
		const isLastSupport = supportLang.some((item) => {
			return lastLang.toLowerCase().indexOf(item.langSet) > -1;
		});

		if (isLastSupport) {
			lang = lastLang;
		} else {
			supportLang.filter((item) => {
				if (browserLang.toLowerCase().indexOf(item.langSet) > -1) {
					lang = item.id;
					return true;
				}
				return false;
			});
		}
		lang = lang || 'en-US';
		return lang;
	}

	public setLocale(lang) {
		this.locale = lang.id;
		this.setLangObject(this.locale);
		location.reload();
	}

	// 根据后端查询到的 langSet显示语言选项
	public showExtracsLang(lanSet) {
		// 在langset里面得语言选项将被显示
		lanSet.forEach((lang) => {
			for (const item of this.langOptions) {
				if (item.langSet === lang) {
					item.hidden = false;
				}
			}
		});
	}

	public autoExtracsLang(lanSet) {
		this.langOptions.forEach((lang) => {
			if (lanSet.indexOf(lang.langSet) > -1) {
				lang.hidden = false;
			} else {
				lang.hidden = true;
			}
		});
	}

	// 设置当选选中的语言对象
	public setLangObject(locale) {
		this.activeLanguage = this.langOptions.filter((item) => locale === item.id)[0];
		localStorage.setItem('locale', locale);
	}
}
