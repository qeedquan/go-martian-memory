package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"math"
	"math/rand"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
	"time"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
	"github.com/qeedquan/go-media/sdl/sdlmixer"
)

const (
	WIDTH  = 640
	HEIGHT = 480
)

const (
	MENU = iota
	HISCORE
	GAME
	OVER
)

var (
	conf struct {
		assets     string
		pref       string
		fullscreen bool
		sound      bool
		music      bool
		cheat      bool
	}

	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	fps      sdlgfx.FPSManager

	sfont2 *image.RGBA

	state   int
	menu    *Menu
	hiscore *Hiscore
	game    *Game
	over    *Over
)

func main() {
	runtime.LockOSThread()
	parseFlags()
	initSDL()
	loadAssets()

	state = MENU
	for {
		event()
		update()
		blit()
		fps.Delay()
	}
}

func parseFlags() {
	conf.assets = filepath.Join(sdl.GetBasePath(), "assets")
	conf.pref = sdl.GetPrefPath("", "martian_memory")
	flag.StringVar(&conf.assets, "assets", conf.assets, "assets directory")
	flag.StringVar(&conf.pref, "pref", conf.pref, "pref directory")
	flag.BoolVar(&conf.fullscreen, "fullscreen", false, "fullscreen mode")
	flag.BoolVar(&conf.sound, "sound", true, "enable sound")
	flag.BoolVar(&conf.music, "music", true, "enable music")
	flag.BoolVar(&conf.cheat, "cheat", false, "infinite retries")
	flag.Parse()
}

func initSDL() {
	err := sdl.Init(sdl.INIT_EVERYTHING &^ sdl.INIT_AUDIO)
	ck(err)

	err = sdl.InitSubSystem(sdl.INIT_AUDIO)
	ek(err)

	err = sdlmixer.OpenAudio(44100, sdl.AUDIO_S16, 2, 8192)
	ek(err)

	_, err = sdlmixer.Init(sdlmixer.INIT_OGG)
	ek(err)

	sdlmixer.AllocateChannels(128)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")

	w, h := WIDTH, HEIGHT
	wflag := sdl.WINDOW_RESIZABLE
	if conf.fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))

	window.SetTitle("Martian Memory")
	renderer.SetLogicalSize(w, h)
	setIcon("gfx/icon.bmp")

	fps.Init()
	fps.SetRate(60)
}

func loadAssets() {
	menu = newMenu()
	hiscore = newHiscore()
	game = newGame()
	over = newOver()
	sfont2 = loadImage("fonts/score2.T8", sdlcolor.Green)
}

func setIcon(name string) {
	name = filepath.Join(conf.assets, name)
	surface, err := sdlimage.LoadSurfaceFile(name)
	if ek(err) {
		return
	}
	window.SetIcon(surface)
	surface.Free()
}

func loadImage(name string, keys ...color.Color) *image.RGBA {
	name = filepath.Join(conf.assets, name)
	f, err := os.Open(name)
	ck(err)
	defer f.Close()

	r := bufio.NewReader(f)
	h := struct {
		Magic   [6]uint8
		Width   uint16
		Height  uint16
		Palette [768]uint8
	}{}
	err = binary.Read(r, binary.BigEndian, &h)
	ck(err)

	if string(h.Magic[:]) != "T-1000" {
		ck(fmt.Errorf("%s: invalid image header %q", name, h.Magic))
	}

	p := make([]byte, int(h.Width)*int(h.Height)*4)
	i := 0
	for y := 0; y < int(h.Height); y++ {
		for x := 0; x < int(h.Width); x++ {
			c, err := r.ReadByte()
			ck(err)

			n := int(c) * 3
			p[i] = h.Palette[n]
			p[i+1] = h.Palette[n+1]
			p[i+2] = h.Palette[n+2]
			p[i+3] = 255
			i += 4
		}
	}

	m := &image.RGBA{
		Rect:   image.Rect(0, 0, int(h.Width), int(h.Height)),
		Stride: int(h.Width) * 4,
		Pix:    p,
	}
	for _, key := range keys {
		m = imageutil.ColorKey(m, key)
	}

	return m
}

func loadSound(name string) *sdlmixer.Chunk {
	name = filepath.Join(conf.assets, name)
	chunk, err := sdlmixer.LoadWAV(name)
	ek(err)
	return chunk
}

func loadMusic(name string) *sdlmixer.Music {
	name = filepath.Join(conf.assets, name)
	mus, err := sdlmixer.LoadMUS(name)
	ek(err)
	return mus
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch ev.(type) {
		case sdl.QuitEvent:
			os.Exit(0)
		}

		switch state {
		case MENU:
			menu.Event(ev)
		case HISCORE:
			hiscore.Event(ev)
		case GAME:
			game.Event(ev)
		case OVER:
			over.Event(ev)
		}
	}
}

func update() {
	switch state {
	case GAME:
		game.Update()
	}
}

func blit() {
	switch state {
	case MENU:
		menu.Blit()
	case HISCORE:
		hiscore.Blit()
	case GAME:
		game.Blit()
	case OVER:
		over.Blit()
	}

	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func ck(err error) {
	if err != nil {
		sdl.LogCritical(sdl.LOG_CATEGORY_APPLICATION, "%v", err)
		sdl.ShowSimpleMessageBox(sdl.MESSAGEBOX_ERROR, "Error", err.Error(), window)
		os.Exit(1)
	}
}

func ek(err error) bool {
	if err != nil {
		sdl.LogError(sdl.LOG_CATEGORY_APPLICATION, "%v", err)
		return true
	}
	return false
}

func tprint(f *image.RGBA, x, y int, format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	line := 0
	char := 0

	r := f.Bounds()
	w, h := r.Dx()/91, r.Dy()

	for _, ch := range text {
		if ch == 94 {
			line++
			char = -1
		}

		px := x + char*w
		py := y + line*h
		fx := int(ch-32) * w
		draw.Draw(canvas, image.Rect(px, py, px+w, py+h), f, image.Pt(fx, 0), draw.Over)
		char++
	}
}

func playMusic(mus *sdlmixer.Music) {
	if !conf.music || mus == nil {
		return
	}
	mus.Play(-1)
}

func playSound(chunk *sdlmixer.Chunk) {
	if !conf.sound || chunk == nil {
		return
	}
	chunk.PlayChannel(1, 0)
}

func toggleFullscreen() {
	conf.fullscreen = !conf.fullscreen
	if conf.fullscreen {
		window.SetFullscreen(sdl.WINDOW_FULLSCREEN_DESKTOP)
	} else {
		window.SetFullscreen(0)
	}
}

type Menu struct {
	back  *image.RGBA
	afont *image.RGBA
	ifont *image.RGBA
	sound *sdlmixer.Chunk
	pos   int
}

func newMenu() *Menu {
	return &Menu{
		back:  loadImage("gfx/menuback.T8"),
		afont: loadImage("fonts/menu_a.T8", sdlcolor.Green),
		ifont: loadImage("fonts/menu_i.T8", sdlcolor.Green),
		sound: loadSound("sounds/option.wav"),
	}
}

func (m *Menu) warpMouse() {
	ow, oh, _ := renderer.OutputSize()
	v := renderer.Viewport()
	lx := func(x int) int {
		nx := int(ga.LinearRemap(float64(x), 0, WIDTH, float64(v.X), float64(ow)-float64(v.X)))
		return nx
	}

	ly := func(y int) int {
		ny := int(ga.LinearRemap(float64(y), 0, HEIGHT, float64(v.Y), float64(oh)-float64(v.Y)))
		return ny
	}

	switch m.pos {
	case 0:
		sdl.WarpMouseInWindow(window, lx((389+280)/2), ly((200+227)/2))
	case 1:
		sdl.WarpMouseInWindow(window, lx((168+473)/2), ly((235+262)/2))
	case 2:
		sdl.WarpMouseInWindow(window, lx((252+417)/2), ly((270+297)/2))
	case 3:
		sdl.WarpMouseInWindow(window, lx((280+389)/2), ly((305+332)/2))
	}
}

func (m *Menu) button() {
	switch m.pos {
	case 0:
		game.Reset()
		state = GAME
	case 1:
		toggleFullscreen()
	case 2:
		ek(hiscore.Load())
		state = HISCORE
	case 3:
		os.Exit(0)
	}
}

func (m *Menu) Event(ev sdl.Event) {
	oldPos := m.pos
	switch ev := ev.(type) {
	case sdl.MouseMotionEvent:
		switch {
		case 280 <= ev.X && ev.X <= 389 && 200 <= ev.Y && ev.Y <= 227:
			m.pos = 0
		case 168 <= ev.X && ev.X <= 473 && 235 <= ev.Y && ev.Y <= 262:
			m.pos = 1
		case 252 <= ev.X && ev.X <= 417 && 270 <= ev.Y && ev.Y <= 297:
			m.pos = 2
		case 280 <= ev.X && ev.X <= 389 && 305 <= ev.Y && ev.Y <= 332:
			m.pos = 3
		}

	case sdl.KeyDownEvent:
		switch ev.Sym {
		case sdl.K_ESCAPE:
			os.Exit(0)

		case sdl.K_DOWN:
			if m.pos++; m.pos > 3 {
				m.pos = 0
			}
			m.warpMouse()
		case sdl.K_UP:
			if m.pos--; m.pos < 0 {
				m.pos = 3
			}
			m.warpMouse()
		case sdl.K_SPACE, sdl.K_RETURN:
			if !ev.Repeat {
				m.button()
			}
		}

	case sdl.MouseButtonDownEvent:
		if ev.Button == 1 {
			m.button()
		}
	}

	if oldPos != m.pos {
		playSound(m.sound)
	}
}

func (m *Menu) Blit() {
	draw.Draw(canvas, canvas.Bounds(), m.back, image.ZP, draw.Src)

	const spacing = 35
	options := []string{
		"    PLAY   ",
		"SCREEN MODE",
		"   SCORES  ",
		"    EXIT   ",
	}

	x := 166
	y := 200
	for i, o := range options {
		if m.pos == i {
			tprint(m.afont, x, y+i*spacing, o)
		} else {
			tprint(m.ifont, x, y+i*spacing, o)
		}
	}
}

type Score struct {
	Name  string
	Time  uint16
	Score uint32
}

type ScoreSlice []Score

func (p ScoreSlice) Len() int      { return len(p) }
func (p ScoreSlice) Swap(i, j int) { p[i], p[j] = p[j], p[i] }
func (p ScoreSlice) Less(i, j int) bool {
	if p[i].Score > p[j].Score {
		return true
	}
	return p[i].Score == p[j].Score && p[i].Time < p[j].Time
}

type Hiscore struct {
	Scores [10]Score
}

func newHiscore() *Hiscore {
	return &Hiscore{}
}

func (h *Hiscore) Insert(score Score) {
	h.Scores[9] = score
	sort.Stable(ScoreSlice(h.Scores[:]))
}

func (h *Hiscore) Load() error {
	name := filepath.Join(conf.pref, "scores.dat")
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	for i := range h.Scores {
		s := &h.Scores[i]
		v := struct {
			Name  [10]byte
			Time  [2]byte
			Score [3]byte
		}{}

		binary.Read(f, binary.LittleEndian, &v)

		s.Name = strings.TrimRight(string(v.Name[:]), "\x00")
		s.Time = uint16(v.Time[0])<<8 | uint16(v.Time[1])
		s.Score = uint32(v.Score[0])<<16 | uint32(v.Score[1])<<8 | uint32(v.Score[2])
	}

	return nil
}

func (h *Hiscore) Save() error {
	name := filepath.Join(conf.pref, "scores.dat")
	f, err := os.Create(name)
	if err != nil {
		return err
	}

	w := bufio.NewWriter(f)
	for _, s := range h.Scores {
		v := struct {
			Name  [10]byte
			Time  [2]byte
			Score [3]byte
		}{}

		for i := range v.Name {
			if i < len(s.Name) {
				v.Name[i] = s.Name[i]
			}
		}
		v.Time = [2]byte{byte(s.Time >> 8), byte(s.Time)}
		v.Score = [3]byte{byte(s.Score >> 16), byte(s.Score >> 8), byte(s.Score)}

		binary.Write(w, binary.LittleEndian, &v)
	}

	err = w.Flush()
	xerr := f.Close()
	if err == nil {
		err = xerr
	}

	if err == nil {
		sdl.Log("Saved successfully to %q", name)
	}

	return err
}

func (h *Hiscore) Event(ev sdl.Event) {
	switch ev := ev.(type) {
	case sdl.MouseButtonDownEvent:
		if ev.Button == 3 {
			state = MENU
		}

	case sdl.KeyDownEvent:
		switch ev.Sym {
		case sdl.K_ESCAPE, sdl.K_BACKSPACE, sdl.K_SPACE, sdl.K_RETURN:
			state = MENU
		}
	}
}

func (h *Hiscore) Blit() {
	b := canvas.Bounds()
	R := 50.0
	G := 130.0
	B := 180.0
	for y := b.Min.Y; y < b.Max.Y; y++ {
		C := sdl.Color{uint8(R), uint8(G), uint8(B), 255}
		draw.Draw(canvas, image.Rect(0, y, b.Dx(), y+1), image.NewUniform(C), image.ZP, draw.Src)

		R -= 0.33
		G -= 0.25
		B -= 0.50
		if R < 0 {
			R = 0
		}
		if G < 0 {
			G = 0
		}
		if B < 0 {
			B = 0
		}
	}

	tprint(sfont2, 20, 25, "   MARTIAN MEMORY     TOP TEN    ")
	tprint(sfont2, 20, 55, "NAME           TIME        SCORE ")
	tprint(sfont2, 20, 75, "---------------------------------")

	tprint(sfont2, 20, 400, "---------------------------------")
	tprint(sfont2, 20, 425, "    PRESS RIGHT MOUSE BUTTON     ")
	tprint(sfont2, 20, 450, "            TO RETURN            ")

	for i, s := range h.Scores {
		if s.Name == "" {
			s.Name = "-"
		}
		tprint(sfont2, 5, 100+25*i, "%s", s.Name)

		min := s.Time / 60
		sec := s.Time - min*60
		tprint(sfont2, 290, 100+25*i, "%d:%02d", min, sec)

		tprint(sfont2, 510, 100+25*i, "%d", s.Score)
	}
}

type Chip struct {
	Type  int
	Side  int
	State int
}

type Particle struct {
	X, Y, W, H float64
	R, G, B    float64
	SpeedX     float64
	SpeedY     float64
	Accel      float64
}

type Game struct {
	touch      *sdlmixer.Chunk
	match      *sdlmixer.Chunk
	noMatch    *sdlmixer.Chunk
	combo      *sdlmixer.Chunk
	music      *sdlmixer.Music
	schips     *image.RGBA
	sfont      *image.RGBA
	bfont      *image.RGBA
	bars       []Particle
	stars      []Particle
	particles  []Particle
	chips      [][]Chip
	turn       int
	frame      uint32
	lastFrame  uint32
	time       float64
	dt         float64
	showTime   float64
	storeX     int
	storeY     int
	matchX     int
	matchY     int
	comboChain uint32
	score      uint32
}

func newGame() *Game {
	return &Game{
		touch:   loadSound("sounds/touch.wav"),
		match:   loadSound("sounds/match.wav"),
		noMatch: loadSound("sounds/nomatch.wav"),
		combo:   loadSound("sounds/combo.wav"),
		music:   loadMusic("music/ingame.ogg"),
		schips:  loadImage("gfx/chips.T8", sdlcolor.Green),
		sfont:   loadImage("fonts/score.T8", sdlcolor.Green),
		bfont:   loadImage("fonts/bonus.T8", sdlcolor.Green),
	}
}

func (c *Game) Reset() {
	rand.Seed(time.Now().UnixNano())
	playMusic(c.music)
	c.resetBars()
	c.resetStars()
	c.resetChips()
	c.frame = sdl.GetTicks()
	c.lastFrame = c.frame
	c.dt = 0
	c.time = 0
	c.showTime = 0
	c.turn = 0
	c.storeX = -1
	c.storeY = -1
	c.matchX = -1
	c.matchY = -1
	c.score = 0
	c.comboChain = 0
}

const (
	chipY        = 4
	chipX        = 8
	chipWidth    = 64
	chipHeight   = 96
	chipStartX   = 36
	chipStartY   = 8
	chipSpacingX = 8
	chipSpacingY = 8
)

func (c *Game) resetChips() {
	c.chips = make([][]Chip, chipY)
	for i := range c.chips {
		c.chips[i] = make([]Chip, chipX)
	}

	for i := 0; i < 2; i++ {
		for j := 1; j <= chipX*chipY/2; j++ {
			for {
				x := rand.Intn(chipX)
				y := rand.Intn(chipY)
				if c.chips[y][x].Type == 0 {
					c.chips[y][x].Type = j
					break
				}
			}
		}
	}
}

const (
	barTopSpeed = 340
	barWidth    = WIDTH
	barHeight   = 10
	floor       = 420

	totalStars = 256
	starSpeed  = 50
)

func (c *Game) resetBars() {
	c.bars = []Particle{
		{
			X:      0,
			Y:      100,
			W:      barWidth,
			H:      1,
			Accel:  150,
			SpeedX: 0,
			R:      50,
			G:      0,
			B:      0,
		},
		{
			X:      0,
			Y:      150,
			W:      barWidth,
			H:      1,
			Accel:  150,
			SpeedX: 0,
			R:      0,
			G:      50,
			B:      0,
		},
		{
			X:      0,
			Y:      200,
			W:      barWidth,
			H:      1,
			Accel:  150,
			SpeedX: 0,
			R:      0,
			G:      0,
			B:      50,
		},
		{
			X:      0,
			Y:      250,
			W:      barWidth,
			H:      1,
			Accel:  150,
			SpeedX: 0,
			R:      10,
			G:      10,
			B:      10,
		},
	}
}

func (c *Game) resetStars() {
	speed := 1
	c.stars = make([]Particle, totalStars)
	for i := range c.stars {
		p := &c.stars[i]
		switch speed {
		case 1:
			p.R, p.G, p.B = 49, 62, 71
		case 2:
			p.R, p.G, p.B = 86, 109, 124
		case 3:
			p.R, p.G, p.B = 130, 162, 183
		case 4:
			p.R, p.G, p.B = 213, 227, 237
		}
		p.X = rand.Float64() * WIDTH
		p.Y = rand.Float64() * floor
		p.SpeedX = float64(speed)

		if speed++; speed > 4 {
			speed = 1
		}
	}
}

func (c *Game) timing() {
	c.frame = sdl.GetTicks()
	c.dt = float64(c.frame-c.lastFrame) / 1000.0
	c.lastFrame = c.frame
	if c.dt > 0.2 {
		c.dt = 0.01
	}

	if c.turn != 2 && c.time < math.MaxUint16 {
		c.time += c.dt
	} else {
		c.showTime += c.dt
	}
}

func (c *Game) Event(ev sdl.Event) {
	switch ev := ev.(type) {
	case sdl.KeyDownEvent:
		switch ev.Sym {
		case sdl.K_ESCAPE:
			sdlmixer.HaltMusic()
			state = MENU
		}

	case sdl.MouseButtonDownEvent:
		if c.turn != 2 {
			c.checkCollision(int(ev.X), int(ev.Y))
		}
	}
}

func (c *Game) checkCollision(x, y int) {
	x -= chipStartX
	y -= chipStartY
	for yy := range c.chips {
		for xx := range c.chips[yy] {
			if x >= xx*(chipWidth+chipSpacingX) &&
				x <= xx*(chipWidth+chipSpacingX)+chipWidth &&
				y >= yy*(chipHeight+chipSpacingY) &&
				y <= yy*(chipHeight+chipSpacingY)+chipHeight {
				c.flipSide(xx, yy)
			}
		}
	}
}

func (c *Game) flipSide(x, y int) {
	p := &c.chips[y][x]
	if p.Side != 0 {
		return
	}

	p.Side = 1
	playSound(c.touch)

	c.storeX = x
	c.storeY = y
	switch c.turn {
	case 0:
		c.matchX = x
		c.matchY = y
		c.turn = 1
	case 1:
		q := &c.chips[c.matchY][c.matchX]
		p.Side = 1
		if p.Type == q.Type || conf.cheat {
			p.State = 1
			q.State = 1
			c.score += 10
			playSound(c.match)
			c.turn = 0
			c.score += c.comboChain * 30
			if c.comboChain++; c.comboChain > 1 {
				c.initCombo(208, 200, 320, 80)
				c.turn = 2
			}
		} else {
			playSound(c.noMatch)
			c.comboChain = 0
			c.showTime = 0
			c.turn = 2
		}
	}
}

const (
	totalParticles   = 2048
	particleAccel    = 70
	particleMaxSpeed = 200
)

func (c *Game) initCombo(x, y, w, h int) {
	for i := range c.particles {
		c.particles[i] = Particle{
			X:      float64(rand.Intn(w) + x),
			Y:      float64(rand.Intn(h) + y),
			SpeedX: float64(rand.Intn(particleMaxSpeed*2) - particleMaxSpeed),
			SpeedY: float64(rand.Intn(particleMaxSpeed*2) - particleMaxSpeed),
			R:      255,
			G:      255,
			B:      255,
		}
	}
}

func (c *Game) updateCombo() {
	for i := range c.particles {
		p := &c.particles[i]
		if p.SpeedX > 0 {
			p.SpeedX -= particleAccel * c.dt
		} else {
			p.SpeedX += particleAccel * c.dt
		}
		p.SpeedY += particleAccel * c.dt
		p.SpeedX = ga.Clamp(p.SpeedX, -particleMaxSpeed, particleMaxSpeed)
		p.SpeedY = ga.Clamp(p.SpeedY, p.SpeedY, particleMaxSpeed)
		p.X += p.SpeedX * c.dt
		p.Y += p.SpeedY * c.dt
	}
}

func (c *Game) updateBars() {
	for i := range c.bars {
		b := &c.bars[i]
		b.SpeedX += b.Accel * c.dt
		b.Y += b.SpeedX * c.dt
		if b.Y >= floor-barHeight*2 {
			b.SpeedX = -barTopSpeed
		}
	}
}

func (c *Game) updateStars() {
	for i := range c.stars {
		s := &c.stars[i]
		s.X += starSpeed * s.SpeedX * c.dt
		if s.X >= WIDTH {
			s.X = 0
		}
	}
}

func (c *Game) Update() {
	c.timing()
	if c.turn == 2 && c.showTime >= 1 {
		c.turn = 0
		c.chips[c.storeY][c.storeX].Side = 0
		c.chips[c.matchY][c.matchX].Side = 0
	}
	c.updateBars()
	c.updateStars()
	if c.turn == 2 && c.comboChain > 1 {
		c.updateCombo()
	}
	if c.isComplete() {
		c.Blit()
		if c.checkHiscore() >= 0 {
			state = OVER
		} else {
			ek(hiscore.Load())
			state = HISCORE
		}
	}
}

func (c *Game) checkHiscore() int {
	for i, s := range hiscore.Scores {
		if c.score > s.Score || (c.score == s.Score && c.time < float64(s.Time)) {
			return i
		}
	}
	return -1
}

func (c *Game) blitStars() {
	for i := range c.stars {
		s := &c.stars[i]

		X := int(s.X)
		Y := int(s.Y)
		C := sdl.Color{uint8(s.R), uint8(s.G), uint8(s.B), 255}
		draw.Draw(canvas, image.Rect(X, Y, X+1, Y+1), image.NewUniform(C), image.ZP, draw.Over)
	}
}

func (c *Game) blitBars() {
	for i := range c.bars {
		b := &c.bars[i]
		R := b.R
		G := b.G
		B := b.B
		for y := int(b.Y); y < int(b.Y)+barHeight; y++ {
			R += 255 / barHeight
			G += 255 / barHeight
			B += 255 / barHeight
			if R > 255 {
				R = 255
			}
			if G > 255 {
				G = 255
			}
			if B > 255 {
				B = 255
			}

			C := sdl.Color{uint8(R), uint8(G), uint8(B), 255}
			X := int(b.X)
			Y := y
			W := int(b.W)
			H := int(b.H)
			draw.Draw(canvas, image.Rect(X, Y, X+W, Y+H), image.NewUniform(C), image.ZP, draw.Over)

			Y += (barHeight-1)*2 - (y-int(b.Y))*2
			draw.Draw(canvas, image.Rect(X, Y, X+W, Y+H), image.NewUniform(C), image.ZP, draw.Over)
		}
	}
}

func (c *Game) blitParticles() {
	for i := range c.particles {
		p := &c.particles[i]
		X := int(p.X)
		Y := int(p.Y)
		C := sdl.Color{uint8(p.R), uint8(p.G), uint8(p.B), 255}
		draw.Draw(canvas, image.Rect(X, Y, X+2, Y+2), image.NewUniform(C), image.ZP, draw.Over)
	}
}

func (c *Game) blitPanel() {
	min := int(c.time / 60)
	sec := int(c.time - float64(min)*60)
	tprint(c.sfont, 10, floor+32, "TIME: %d:%02d", min, sec)
	tprint(c.sfont, 300, floor+32, "SCORE: %d", c.score)
}

func (c *Game) blitChips() {
	for y := range c.chips {
		for x := range c.chips[y] {
			dx := chipStartX + x*(chipWidth+chipSpacingX)
			dy := chipStartY + y*(chipHeight+chipSpacingY)
			sy := 0

			if c.chips[y][x].State == 1 {
				c.chips[y][x].Side = 1
			}

			if c.chips[y][x].Side != 0 {
				sy = c.chips[y][x].Type * chipHeight
			}

			draw.Draw(canvas, image.Rect(dx, dy, dx+chipWidth, dy+chipHeight), c.schips, image.Pt(0, sy), draw.Over)
		}
	}
}

func (c *Game) blitCombo() {
	o := 0
	bonus := ""
	switch c.comboChain {
	case 2:
		bonus = "  NICE   "
	case 3:
		bonus = "  GREAT  "
	case 4:
		bonus = "EXCELLENT"
	default:
		bonus = "TERRIFIC "
	}
	x := 208
	y := 200

	tprint(c.bfont, x+o, y, "%s", bonus)
	tprint(c.bfont, x, y+40, "%dX Combo", c.comboChain)
}

func (c *Game) Blit() {
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Src)
	c.blitStars()
	c.blitBars()
	c.blitPanel()
	c.blitChips()
	if c.turn == 2 && c.comboChain > 1 {
		c.blitParticles()
		c.blitCombo()
	}
}

func (c *Game) isComplete() bool {
	for y := range c.chips {
		for x := range c.chips[y] {
			if c.chips[y][x].State == 0 {
				return false
			}
		}
	}
	return true
}

type Over struct {
	name string
	pos  int
}

func newOver() *Over {
	return &Over{}
}

func (o *Over) Reset() {
	o.name = ""
	o.pos = 0
}

func (o *Over) Event(ev sdl.Event) {
	switch ev := ev.(type) {
	case sdl.KeyDownEvent:
		if ev.Sym > 41 && o.pos < 10 {
			if ev.Sym >= 97 && ev.Sym <= 122 {
				o.name += string(ev.Sym - 32)
			} else {
				o.name += string(ev.Sym)
			}
			o.pos++
		} else if ev.Sym == sdl.K_RETURN || ev.Sym == sdl.K_ESCAPE {
			hiscore.Insert(Score{
				Name:  o.name,
				Time:  uint16(game.time),
				Score: game.score,
			})
			hiscore.Save()
			ek(hiscore.Save())
			state = HISCORE
		} else if ev.Sym == sdl.K_BACKSPACE && o.pos > 0 {
			o.name = o.name[:len(o.name)-1]
			o.pos--
		}
	}
}

func (o *Over) Blit() {
	x := 175
	y := 220
	draw.Draw(canvas, image.Rect(x, y, x+290, y+52), image.NewUniform(color.White), image.ZP, draw.Over)
	tprint(sfont2, x+10, y+5, "ENTER YOUR NAME")
	tprint(sfont2, x+51+o.pos*18, y+29, ".")
	tprint(sfont2, x+51, y+24, "%s", o.name)
}
