import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-power-info-card',
  templateUrl: './power-info-card.component.html',
  styleUrls: ['./power-info-card.component.scss']
})
export class PowerInfoCardComponent implements OnInit {

  constructor() { }

  @Input() powerInfoCard;

  ngOnInit(): void {
  }

}
