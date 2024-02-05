import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';

import { CommonService } from 'src/app/common-app/service';
import { VideoPlayerComponent } from './video-player.component';
import { VideoRoutingModule } from './video-routing.module';
import { TiSliderModule, TiMessageModule } from '@cloud/tiny3';


@NgModule({
	declarations: [VideoPlayerComponent],
	imports: [
		CommonModule,
		FormsModule,
        HttpClientModule,
        TiMessageModule,
        TiSliderModule,
		TranslateModule,
        VideoRoutingModule
	],
	providers: [CommonService]
})
export class VideoPlayerModule { }
