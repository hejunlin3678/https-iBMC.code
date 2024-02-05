import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';

@Injectable()
export class TimeoutService {
	constructor() {

	}
	public error401 = new Subject<any>();
}
